The (index.html) pages of both Java and C++ containing examples have been generated 
from markdown files, i.e., eidlibJava_Wrapper/index.md and eidlib/index.md respectively.
Remarkable has been used to transform markdown to html, however any other tool will be similar.


Java and C++ documentation generation 
- use gen_docs.sh to generate the documentation using Doxygen for C++ tool and javadocs for Java
- The C++ configuration file is called "Doxygen" while Java uses a cli command.

The zip files javadocs.zip and cppdocs.zip (the result of executing gen_docs.sh) are added to instalation folder