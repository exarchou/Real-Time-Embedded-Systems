# Real-Time Embedded Systems

This repository contains the assignments for the Academic Courses "Real-Time Embedded Systems" taught in the Spring of 2019-2020 in Electrical and Computer Engineering Department of Aristotle University of Thessaloniki.

The purpose of this project is the familiarization with **pthreads** library of **C** through the development a **producer-consumer** variant. The work is divided into two deliverables, which are responsible for optimizing the number of consumer threads to minimize in-queue time of each item and creating a timer to execute items under a specific frequency. 

At the end, the source code will be mounted on a **Raspberry-Pi** and the experiments will take place Real-Time in the campus area.



### 1.1. Average Time in Queue ~ Number of Consumer Threads

This report analyzes the implementation of a **FIFO circular queue**, which contains pointers to functions and arguments, as a variation of the **producer-consumer problem**, using **pthreads**, as well as the results of measuring the time each item remains in the queue. A list of pointers to functions and pointers to arguments has been created, as global variables for each thread to access, which make a simple printout or calculate a mathematical function. The main function generates P threads of producers and Q threads of consumers and displays the average time that an item is stored in the queue from the time a producer places it until a consumer receives it. The producer randomly selects a function and an argument and places them in the queue. The consumer receives the first element of the queue, counts the time and executes it. At this point the mechanism of termination of these threads must be understood. This is done by receiving in consumer a pointer to NULL from the queue. When a consumer reads it, he puts it back in queue for the next one and terminates. 

The aim is to find the number of consumer threads that minimizes the above statistics. The number of producers is constant and equal to 1, while the number of LOOP iterations of the producer was set to 10,000, in order to extract secure statistical data. Finally, the queue size was set equal to 10, as the consumption rate is higher than the production rate and therefore it is rare for the buffer to fill up. Before analyzing the applied methodology it should be mentioned that for the execution of the program a processor consisting of 4 physical cores was used, each of which can run simultaneously as 2 threads, ie 8 logical cores, in case there is no waiting for input -exit. Because the functions we used include printing, the optimal number of consumer threads will be more than 8, as the processor will schedule new tasks during the input-output process in order to increase its efficiency.



### 1.2. Results

Finding the minimum average in-queue time requires performing the experiment on different numbers of consumer threads (Q = 2,4,6,…, 60). The following graph made with the help of MATLAB illustrates the average value of the in-queue time, depending on the number of consumers.



<p allign = 'center'>
    <img src = 'images/diagram1.png' width = '70%'>
</p>



| **Cons** | **Mean** | **St. Dev** | **Median** | **Min** | **Max** |      | **Cons** | **Mean** | **St. Dev** | **Median** | **Min** | **Max** |
| -------- | -------- | ----------- | ---------- | ------- | ------- | ---- | -------- | -------- | ----------- | ---------- | ------- | ------- |
| 2        | 46.858   | 43.452      | 37         | 0       | 647     |      | 32       | 2.5864   | 2.5356      | 3          | 0       | 64      |
| 4        | 25.527   | 93.333      | 18         | 0       | 2917    |      | 34       | 2.708    | 2.6204      | 3          | 0       | 62      |
| 6        | 7.938    | 13.384      | 3          | 0       | 160     |      | 36       | 2.7109   | 2.5281      | 3          | 0       | 48      |
| 8        | 3.1719   | 6.0669      | 3          | 0       | 376     |      | 38       | 2.8011   | 5.187       | 3          | 0       | 373     |
| 10       | 2.7379   | 3.097       | 3          | 0       | 78      |      | 40       | 3.0537   | 4.9934      | 3          | 0       | 178     |
| 12       | 2.7147   | 3.0211      | 3          | 0       | 95      |      | 42       | 2.7137   | 2.8796      | 3          | 0       | 71      |
| 14       | 2.7125   | 4.7912      | 3          | 0       | 407     |      | 44       | 2.6964   | 2.3052      | 3          | 0       | 52      |
| 16       | 2.7189   | 2.1984      | 3          | 0       | 74      |      | 46       | 2.6648   | 2.2114      | 3          | 0       | 133     |
| 18       | 2.8442   | 5.9883      | 3          | 0       | 483     |      | 48       | 2.6944   | 3.1094      | 3          | 0       | 80      |
| 20       | 2.7155   | 2.2759      | 3          | 0       | 45      |      | 50       | 2.6562   | 1.9723      | 3          | 0       | 42      |
| 22       | 2.7105   | 2.8008      | 3          | 0       | 93      |      | 52       | 2.6614   | 2.5789      | 3          | 0       | 57      |
| 24       | 2.6584   | 2.8906      | 3          | 0       | 62      |      | 54       | 2.7677   | 3.1054      | 3          | 0       | 78      |
| 26       | 2.8305   | 4.471       | 3          | 0       | 96      |      | 56       | 2.7671   | 3.1409      | 3          | 0       | 65      |
| 28       | 2.6019   | 2.3088      | 3          | 0       | 87      |      | 58       | 3.0997   | 36.532      | 3          | 0       | 3467    |
| 30       | 2.7483   | 2.4794      | 3          | 0       | 58      |      | 60       | 2.6348   | 1.8056      | 3          | 0       | 42      |



From the above it can be concluded that from one point onwards the increase in consumers does not significantly contribute to the reduction of the average waiting time. Therefore, by choosing a point near the "knee" of the curve, satisfactory results are achieved. This could be the 16 consumers, with whom a small standard deviation is achieved.



### 2.1. Timer and Time Drift correction

In this section, the producer thread places an item in the queue every **Period** seconds, which is achieved through *usleep*. Due to the time needed by other tasks of a producer, it should never sleep for *Period* seconds, but for a variable time, depending on the previous Period. This Period correction is called **Time Drift correction**.