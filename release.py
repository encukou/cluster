#! /usr/bin/python

# This file is not an official part of the source. It's just a reference for how
# the software is currently packaged.

"""
Pack everything and upload it to web.
"cluster.exe" and all needed Windows libraries are expected to be in the windows-binaries/ directory.

Python is used instead of a shell script because of its better error handling.
"""

import shutil
import os
import sys
import zipfile
import tarfile
import bzrlib.commands, bzrlib.builtins, bzrlib.branch

print "Make program & docs..."
os.system('qmake')
os.system('make')
os.system('doxygen')

print "Removing previous data..."
try:
    os.mkdir('release')
except OSError:
    pass

shutil.rmtree('release')

print "Creating branch..."
bzrlib.builtins.cmd_branch().run('.', 'release', standalone=True)

revision = 'r%s' % bzrlib.branch.Branch.open('release').revno()
print "Revision is " + revision

os.mkdir('release/release')

print "Creating source tarball..."
tarball_name = 'release/release/cluster-src.tar.gz'
tar = tarfile.open(tarball_name, 'w:gz')
tar.add('release', 'cluster-' + revision, exclude=lambda x:x=='release/release');
tar.close()

print "Removing tree..."
for file in os.listdir('release'):
    if file not in 'doc release .bzr'.split():
        name = os.path.join('release', file)
        try:
            os.remove(name)
        except OSError:
            shutil.rmtree(name)

print "Copying generated docs..."
shutil.copytree('doc/generated', 'release/doc/generated')

print "Copying bare Windows executable..."
shutil.copy('windows-binaries/cluster.exe', 'release/release')

print "Creating Windows zip file..."
zip = zipfile.ZipFile('release/release/cluster.zip', 'w', compression=zipfile.ZIP_DEFLATED)
srcdir = 'windows-binaries'
destname = 'cluster-' + revision
for dirname, dirs, files in os.walk(srcdir):
    for file in files:
        filename = os.path.join(dirname, file)
        print file + " " * 10 + '\r',
        sys.stdout.flush()
        zip.write(filename, filename.replace(srcdir, destname))
zip.close()

print "Creating redirection webpages..."
redirect_content = """<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <meta http-equiv="refresh" content="2;doc/index.html">
        <title>Clustering visualization software</title>
    </head>
    <body>
        <p>If you are not redirected automatically, click <a href="doc/index.html">here</a></p>
    </body>
</html>"""
#open('release/index.html', 'w').write(redirect_content)
open('release/index.php', 'w').write("<?php header('Location: doc/index.html') ;?>" + redirect_content)

print "Use scp to upload to web"
for file in os.listdir('release'):
    name = os.path.join('release', file)
    os.system('scp -r "%s" "pviktor@cs.joensuu.fi:~/www/cluster"' % name)
