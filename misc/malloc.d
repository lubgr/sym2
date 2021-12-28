
#pragma D option quiet

pid$target::aligned_alloc:entry {
  @[ustack(100)] = count();
  @size[probefunc] = sum(arg1);
  @totalsize = sum(arg1);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::malloc:entry {
  @size[probefunc] = sum(arg0);
  @totalsize = sum(arg0);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::calloc:entry {
  @size[probefunc] = sum(arg0 * arg1);
  @totalsize = sum(arg0 * arg1);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::realloc:entry {
  @size[probefunc] = sum(arg1);
  @totalsize = sum(arg1);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::reallocf:entry {
  @size[probefunc] = sum(arg1);
  @totalsize = sum(arg1);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::valloc:entry {
  @size[probefunc] = sum(arg0);
  @totalsize = sum(arg0);
  @calls[probefunc] = count();
  @totalcalls = count();
}

pid$target::free:entry {
  @frees = count();
}

dtrace:::END {
  printa("%s\t%@16d calls %@16d bytes\n", @calls, @size);
  printa("\ntotal\t%@16d calls %@16d bytes", @totalcalls, @totalsize);
  printa("\nfree\t%@16d calls", @frees);
}
