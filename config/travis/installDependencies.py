import os
import re
import sys
import urllib
import ssl

# figure out where we'll be making the build

for line in open( "SConstruct" ).readlines() :
	if re.search( "gaffer[A-Za-z]*Version = ", line  ) :
		exec( line.strip() )

platform = "osx" if sys.platform == "darwin" else "linux"

buildDir = "build/gaffer-%d.%d.%d.%d-%s" % ( gafferMilestoneVersion, gafferMajorVersion, gafferMinorVersion, gafferPatchVersion, platform )

# get the prebuilt dependencies package and unpack it into the build directory

if platform == 'osx':
	context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
else:
	context = None

downloadURL = "https://github.com/GafferHQ/dependencies/releases/download/0.44.0.0/gafferDependencies-0.44.0.0-" + platform + ".tar.gz"

sys.stderr.write( "Downloading dependencies \"%s\"" % downloadURL )

if platform == 'osx':
        context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
	tarFileName, headers = urllib.urlretrieve( downloadURL, context = context)
else:
        context = None
	tarFileName, headers = urllib.urlretrieve( downloadURL)



os.makedirs( buildDir )
os.system( "tar xf %s -C %s --strip-components=1" % ( tarFileName, buildDir ) )
