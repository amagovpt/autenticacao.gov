Short info -- read ReadMe.doc for more details.

This doc explains how to generate the windows
installer .msi file for the eID test infra project.

To do only once:
  - check out http://10.2.250.30/svn/ThirdParty/wix in ThirdParty\wix
  - install ProjectAggregator2.msi and Wix3-3.0.4415.msi

To generate the .msi:
  When build the _Builds\eidtestinfra.sln in Visual, an
  EidInfraWix.msi is generated in EidInfraWix\bin\Release
  or EidInfraWix\bin\Debug.
