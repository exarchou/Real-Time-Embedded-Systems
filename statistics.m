clear all; clc; close all;

fprintf("===============\n1\n==============\n");
a1 = load("In-Queue-time-1.txt");
fprintf("Timer 1 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(a1), std(a1), min(a1), max(a1));
figure; ctrs = [0:25:200]; histogram(a1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 1 sec");
saveas(gcf,'img/In-Queue-time-1.png')

a2 = load("Timedrift-1.txt");
fprintf("Timer 1 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(a2), std(a2), min(a2), max(a2));
figure; ctrs = [-500:25:500]; histogram(a2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 1 sec");
saveas(gcf,'img/Timedrift-1.png')


fprintf("===============\n2\n==============\n");
b1 = load("In-Queue-time-2.txt");
fprintf("Timer 0.1 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(b1), std(b1), min(b1), max(b1));
figure; ctrs = [0:25:200]; histogram(b1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 0.1 sec");
saveas(gcf,'img/In-Queue-time-2.png')

b2 = load("Timedrift-2.txt");
fprintf("Timer 0.1 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(b2), std(b2), min(b2), max(b2));
figure; ctrs = [-500:25:500]; histogram(b2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 0.1 sec");
saveas(gcf,'img/Timedrift-2.png')


fprintf("===============\n3\n==============\n");
c1 = load("In-Queue-time-3.txt");
fprintf("Timer 0.01 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(c1), std(c1), min(c1), max(c1));
figure; ctrs = [0:25:200]; histogram(c1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 0.01 sec");
saveas(gcf,'img/In-Queue-time-3.png')

c2 = load("Timedrift-3.txt");
fprintf("Timer 0.01 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(c2), std(c2), min(c2), max(c2));
figure; ctrs = [-500:25:500]; histogram(c2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 0.01 sec");
saveas(gcf,'img/Timedrift-3.png')




fprintf("===============\n4: ALL TIMERS!\n==============\n");
d1 = load("In-Queue-time-4-1.txt");
fprintf("Timer 1 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(d1), std(d1), min(d1), max(d1));
figure; ctrs = [0:25:200]; histogram(d1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 1 sec (All timers)");
saveas(gcf,'img/In-Queue-time-4-1.png')

d2 = load("Timedrift-4-1.txt");
fprintf("Timer 1 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(d2), std(d2), min(d2), max(d2));
figure; ctrs = [-500:25:500]; histogram(d2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 1 sec (All timers)");
saveas(gcf,'img/Timedrift-4-1.png')


e1 = load("In-Queue-time-4-2.txt");
fprintf("Timer 0.1 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(e1), std(e1), min(e1), max(e1));
figure; ctrs = [0:25:200]; histogram(e1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 0.1 sec (All timers)");
saveas(gcf,'img/In-Queue-time-4-2.png')

e2 = load("Timedrift-4-2.txt");
fprintf("Timer 0.1 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(e2), std(e2), min(e2), max(e2));
figure; ctrs = [-500:25:500]; histogram(e2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 0.1 sec (All timers)");
saveas(gcf,'img/Timedrift-4-2.png')


f1 = load("In-Queue-time-4-3.txt");
fprintf("Timer 0.01 sec (In-Queue): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(f1), std(f1), min(f1), max(f1));
figure; ctrs = [0:25:200]; histogram(f1, ctrs); xticks(ctrs); xlabel("In-Queue time (usec)"), title("Timer with Period 0.01 sec (All timers)");
saveas(gcf,'img/In-Queue-time-4-3.png')

f2 = load("Timedrift-4-3.txt");
fprintf("Timer 0.01 sec (Timedrift): mean: %.2f usec, std: %.2f, min: %d usec, max: %d usec\n", mean(f2), std(f2), min(f2), max(f2));
figure; ctrs = [-500:25:500]; histogram(f2, ctrs); xlabel("Timedrift (usec)"), title("Timer with Period 0.01 sec (All timers)");
saveas(gcf,'img/Timedrift-4-3.png')










