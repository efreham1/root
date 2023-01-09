# root

Olof Lindström & Fredrik Hammarberg & Viktor Wallstén & Viktor Kangasniemi & Edvin Bruce & David Ohanjanian

2023-01-11

---

## Building and running

To build any c-file in /src with a main function use the command
  `$ make exe/<name>`

To build and run the demo use the command

	`$ make demo`

## Information about the calculator
#### Largest allowed allocation is 2040 bytes.
#### Unions are not explicitly supported.
They can still be used by allocating them as raw data if the union doesn't have a pointer in it and as a struct 
that is only a pointer if the union has a pointer in it. If the second approch is chosen and the union also contains
a long integer there is a risk of the long iteger being overwritten in the garbage collection proccess.

---

##  Tests

To run all tests for the garbage collector use the command

	`$ make test_all`
  
To run the tests for a specific module use the command

	`$ make test_<module_name>`

Both commands runs unit and integration tests.

To run a more demanding integration test use the command

	`$ make stock`
	
Which runs a script for assignment 2 which creates and removes a bunch of data

---
