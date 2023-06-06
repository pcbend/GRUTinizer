#ifndef ROOT_haddCommandLineOptionsHelp
#define ROOT_haddCommandLineOptionsHelp
constexpr static const char kCommandLineOptionsHelp[] = R"RAW(
usage: hadd [-a A] [-k K] [-T T] [-O O] [-v V] [-j J] [-dbg DBG] [-d D] [-n N]
            [-cachesize CACHESIZE]
            [-experimental-io-features EXPERIMENTAL_IO_FEATURES] [-f F]
            [-fk FK] [-ff FF] [-f0 F0] [-f6 F6]
            TARGET SOURCES

OPTIONS:
  -a                                   Append to the output
  -k                                   Skip corrupt or non-existent files, do not exit
  -T                                   Do not merge Trees
  -O                                   Re-optimize basket size when merging TTree
  -v                                   Explicitly set the verbosity level: 0 request no output, 99 is the default
  -j                                   Parallelize the execution in multiple processes
  -dbg                                 Parallelize the execution in multiple processes in debug mode (Does not delete partial files stored inside working directory)
  -d                                   Carry out the partial multiprocess execution in the specified directory
  -n                                   Open at most 'maxopenedfiles' at once (use 0 to request to use the system maximum)
  -cachesize                           Resize the prefetching cache use to speed up I/O operations(use 0 to disable)
  -experimental-io-features            Used with an argument provided, enables the corresponding experimental feature for output trees
  -f                                   Gives the ability to specify the compression level of the target file(by default 4) 
  -fk                                  Sets the target file to contain the baskets with the same compression
                                       as the input files (unless -O is specified). Compresses the meta data
                                       using the compression level specified in the first input or the
                                       compression setting after fk (for example 206 when using -fk206)
  -ff                                  The compression level use is the one specified in the first input
  -f0                                  Do not compress the target file
  -f6                                  Use compression level 6. (See TFile::SetCompressionSettings for the support range of value.)
  TARGET                               Target file
  SOURCES                              Source files
)RAW";
#endif
