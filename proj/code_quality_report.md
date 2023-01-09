# Code Quality Report
We belive that we have written good code. The reasoning for this statment follows in some bulletpoints:
### #1
We have followed the GNU C standard and therefore the code is easy to read.
### #2
We began with the easiest tasks and pushed the harder ones into the future, this ensured that we knew more about the
code and the problem when we evetaully got to the harder tasks.
### #3
We mostly left optimization to the end. Because of this we could focus on writing functioning code instad of fast code.
### #4
All code was either written in a group or explained to a group which meant that bad implementaions and potential
bugs were more easily caught.
### #5
All code is well abstracted into modules and functions. All functions have clear purposes and a name that reflects
that purpose. All variables also has clear names. This means that the code is easy to understand if one understands
C-syntax. The abstraction also helps if the code is to be refactored.
### #6
No unneccasary one-liners or global variables or any other ugly hacks. This ensures readability, maintainability,
and ease of debugging.
### #7
The specification was almost followed completly and the code works very well in all the tests and integrations.
The code also passes all tests in the quite extensive testing-library. This leads us to belive that not only is
the code well written but it is also very functional.
### #8
The entire program is very defensivly programmed. This is done by having multiple asserts in the code that ensures
all parameters are what they are expected to be.
### A Nice Example
```
void *page_alloc_data (page_t *page, unsigned int bytes, bool visitation_bit)
{
  assert (bytes % 8 == 0);

  metadata_t *metadata = alloc_metadata (page);

  *metadata = set_data_size ((unsigned int) bytes);
  *metadata = set_visitation_bit (*metadata, visitation_bit);

  return alloc_mem_block (page, bytes);
}
```
This function is for allocating raw data on a page. This code is very well abstracted, separate functions are used
for all complicated tasks meaning that the function itself becomes vewry simple. It's also very easy to read and
understand the formatting and naming of functions and varaibles.
