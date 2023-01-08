# Test Report

## Testing Overview
The code is primarly tested from several CUint files. Each file has unit tests for a specific module as well as
some simpler integration tests. The more severe integration test is tested via two makefile scripts and uses the
code from assigment 2. Regression testing has been implemented in two ways, first via makefile scripts so that when
"make test_all" is written all new and prior tests are run. Second, via github actions' support for CI. The
automation of these tests were done in the same ways as regression testing, via makefile and github CI.

## Code Coverage Report
```
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                    Branches   Taken  Cover   Missing
------------------------------------------------------------------------------
src/gc.c                                      44      39    88%   12,73,171,184
src/heap.c                                    83      72    86%   7,17,49,113,134,136,143,159,169,242,258
src/metadata.c                                58      42    72%   14,24,38,53,67,83,89,90,94,98,129,162,176,183
src/mover.c                                   60      53    88%   26,69,76,86,132,165,173
src/page.c                                    28      21    75%   14,21,99,110,122,134
src/stack_scanner.c                           12      12   100%   
------------------------------------------------------------------------------
TOTAL                                        285     239    83%
------------------------------------------------------------------------------

------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   Missing
------------------------------------------------------------------------------
src/gc.c                                     122     122   100%   
src/heap.c                                   137     133    97%   75-76,120,149
src/metadata.c                                92      89    96%   131,142,164
src/mover.c                                   81      79    97%   80,187
src/page.c                                    76      76   100%   
src/stack_scanner.c                           20      20   100%   
------------------------------------------------------------------------------
TOTAL                                        528     519    98%
------------------------------------------------------------------------------
```
ll non-executed lines and almost all non-executed branches are code that handles crashing the program when
something is not as it should. This is done via asserts which is difficult to automatically test.

## Most Nasty Bugs
https://github.com/IOOPM-UU/root/issues/9
https://github.com/IOOPM-UU/root/issues/8
https://github.com/IOOPM-UU/root/issues/7
All other bugs were so small and simple that they got resolved as soon as they got discovered.
