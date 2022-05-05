/*
 * Процедура average получает на вход массив вещественных чисел и вовращает их среднее значение.
 * В этом примере ограничено количество чисел значением 200.
 */
const MAXAVGSIZE = 200;
struct input_data { 				/* Тип параметров процедуры */
	double input_data<200>;
};
typedef struct input_data input_data; 		/* Имя типа параметров */
program AVERAGEPROG {
 	version AVERAGEVERS {
 		double AVERAGE(input_data) = 1; /* Номер процедуры */
 	} = 1; 					/* Номер версии */
} = 22855; 					/* Номер программы */