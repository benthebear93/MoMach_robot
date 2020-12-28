### NC CODE SCRIPT ###

LASER_MM = 0.17
MEASURE_SIZE = 25

File = open("C:/Users/UNIST/Dropbox/MS_course/Mobile_machining/machining/sufrcae_test.txt", "w")
print("G90G54G00X0.0Y0.0Z5.0M03F50S0", file=File)
num_of_times = round(MEASURE_SIZE/LASER_MM)

for a in range(0, num_of_times):
    print(a)
    k = str(round(a*0.17, 4))
    print("G90G54G01X" + k + "Y0.0Z5.00", file=File)
    print("G90G54G01X" + k + "Y25.0Z5.0", file=File)
print("M5", file=File)
File.close()

