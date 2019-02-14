The (index.html) pages of both Java and C++ containing examples have been generated 
from markdown files, i.e., eidlibJava_Wrapper/index.md and eidlib/index.md respectively.
Remarkable has been used to transform markdown to html, however any other tool will be similar.

Java and C++ documentation generation 
- QtCreator projects "eidlib.pro" and "eidlibJava_Wrapper.pro" have been configured 
to allow automatic documentation generation.
- Visual Studio projects "pteidlib.2008.vcxproj" and "pteidlibJava.2008.vcxproj" have also been configured
to allow automatic documentation generation as post-build steps.
- The C++ documentation configuration file is called "Doxygen" while Java uses a cli command.

The zip files javadocs.zip and cppdocs.zip (are the result of building respective projects).