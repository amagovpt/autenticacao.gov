{
  'targets': [
    {
      'target_name': 'pteid',
      'sources': [ 
        '../eidlib/eidlibCard.cpp',
        '../eidlib/eidlibCrypto.cpp',       
        '../eidlib/eidlibDoc.cpp',
        '../eidlib/eidlibException.cpp',
        '../eidlib/eidlibReader.cpp',
        '../eidlib/InternalUtil.cpp',
        './GeneratedFiles/eidlibNodeJS_Wrapper.cpp',
      ],
      'include_dirs': [ 
        '../applayer',
        '../dialogs',
        '../common',
        '../cardlayer',
        '../eidlib',
      ],
      'defines': [
        'PTEID_35',
        'EIDMW_EIDLIB_EXPORTS',
        #'SWIGRUNTIME_DEBUG',
      ],
      'conditions': [
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/include/PCSC',
          ],
          'defines': [
            'EIDMW_PREFIX=/usr/local',
          ],
          'link_settings': {
            'libraries': [
              '-lpteidcommon',
              '-lpteiddialogsQT',
              '-lpteidcardlayer',
              '-lpteidapplayer',
              '-lpteidlib',
            ],
            'library_dirs': [
              '../lib',
            ],
          },
          'cflags_cc!': [
            '-fno-rtti', 
            '-fno-exceptions',
           ],
        }],
      ],
    },
  ],
}
