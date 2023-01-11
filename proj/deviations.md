# Deviations

## Format String in Metadata
In the metadata of an object there is no option to store a pointer to a format string. The reason for this choice
was that it is never needed since the format vector has all the necessary informaton and because of this fact it
was not implemented.

## No Metadata for Available for Expansion
In the metadata of an object there is no indication for the availability for expansion. The reason for this choice
was that the need for that indication never arose and it was therefore never implemented.

## Memory Footprint of GC not Contained within Specified Size
When the size of the heap is specified by the user that size is the usable size. The actual size of the heap is
larger and can be obtained via a function.
