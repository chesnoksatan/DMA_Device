#ifndef DMA_H
#define DMA_H

typedef struct gen_sin {
  double A, w, t; // Параметры для генераии синусоиды
  unsigned int byte; // Количество байт для записи
} gen_sin;

// Определение "Магических" чисел для ioctl
#define DMA_IOC_MAGIC 'k'
#define DMA_WRITE_SIN _IOWR(DMA_IOC_MAGIC, 1, struct gen_sin *)
//#define TIME_IOCREAD _IOWR(TIME_IOC_MAGIC, 2, struct my_date *)

/*
  Следующие "магические" числа для разных функций
*/

int write_sin_dma ( struct gen_sin *);

int sinus(int sinu);

#endif
