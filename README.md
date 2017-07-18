# uni_boxpack
C project as prerequisite for the final Info II exam.

## Goal
### Story
Bla bla bla Cleaning robot ... bla inefficient. My task is to improve garbage collection efficiency, by validating given sorting/packing patterns.

### Task
With a given number of storage containers (variable in size) and a given set of (packet, algorithm) tuples my C-program shall determine, whether all packets can be stowed away in the available containers or not.

#### Call
```Shell
boxpack input output
```

#### Input
- Two lines (values space-separated)
  1. Capacities of storage containers
  2. Pairs of fitting-algo and packet-size
- Sizes are decimal numbers
- algorithm is one of:
  * bf (best-fit)
  * nf (next-fit)
  * ff (first-fit)
  * awf (almost-worst-fit)
- algorithm tokens determine the state of the robot, thus apply to all following non-algorithm inputs

#### Output
```Shell
<Index>: <packet_size1> <packet_size2> ... <packet_sizeN>\n
```
- Index starting at 0
- colon after index
- space-separated values
- line-ending \n
- no trailing space after last packet

#### Fitting algorithms
##### First-Fit
Find the first box that still has 'packet-size' space left and insert packet there.

##### Best-Fit
Starting from the first, find the box, that leaves the least space after insertion into a box.

##### Next-Fit
Find the next box (i.e. starting from the one most recently packed) that will fit the current packet.

##### Almost-Worst-Fit
Find the second-most empty box and insert packet there.

## License
The Unlicense. For more information see License.md
