set xdata time
set timefmt "%Y-%m"

set yrange [0:25]

set xtics rotate

set grid

system "cat firstTwoMonths.txt | head -n -3 > firstTwoMonths.tmp"
system "cat firstMonth.txt | head -n -2 > firstMonth.tmp"
system "cat firstWeek.txt | head -n -1 > firstWeek.tmp"

plot "firstTwoMonths.tmp"  using 1:2 with linespoints title "Hours played in first two months of ownership", "firstMonth.tmp"  using 1:2 with linespoints title "Hours played in first month of ownership",  "firstWeek.tmp"  using 1:2 with linespoints title "Hours played in first week of ownership", "firstDay.txt"  using 1:2 with linespoints title "Hours played in first day of ownership", "firstHour.txt"  using 1:2 with linespoints title "Hours played in first hour of ownership"

pause -1

set yrange [0:1.5]

plot "firstTwoMonths.tmp"  using 1:5 with linespoints title "Fraction who quit in first two months of ownership", "firstMonth.tmp"  using 1:5 with linespoints title "Fraction who quit in first month of ownership", "firstWeek.tmp"  using 1:5 with linespoints title "Fraction who quit in first week of ownership", "firstDay.txt"  using 1:5 with linespoints title "Fraction who quit in first day of ownership", "firstHour.txt"  using 1:5 with linespoints title "Fraction who quit in first hour of ownership"

pause -1


set yrange [0:1.8]

plot "hourBins.txt" using 1:2 with linespoints title "Fraction who played > 1 hour", "hourBins.txt" using 1:3 with linespoints title "Fraction who played > 5 hours", "hourBins.txt" using 1:4 with linespoints title "Fraction who played > 10 hours", "hourBins.txt" using 1:5 with linespoints title "Fraction who played > 20 hours", "hourBins.txt" using 1:6 with linespoints title "Fraction who played > 50 hours", "hourBins.txt" using 1:7 with linespoints title "Fraction who played > 100 hours", "hourBins.txt" using 1:8 with linespoints title "Fraction who played > 200 hours", "hourBins.txt" using 1:9 with linespoints title "Fraction who played > 500 hours", "hourBins.txt" using 1:10 with linespoints title "Fraction who played > 1000 hours"

pause -1