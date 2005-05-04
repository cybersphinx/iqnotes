BEGIN {
    p1 = ARGV[1];
    sub(/\.[a-z]+$/, ".png", p1);
    sub(/ /, "", p1);
    command = "convert '" ARGV[1] "' '" p1 "'";
    print "Running : " command;
    system(command);
}