# Mechanical 7 segment Servo Clock on STM32
This is the servo clock project that i worked on for VP.Start company.

![pic](https://user-images.githubusercontent.com/73976219/152266998-6e91aa42-a0c9-4c6d-88e7-9dc7350e52c3.jpg)

## Build process

### Main Material

- Acrylic
- Stainless Steel

### Components

- STM32f401 – [Buy Here](https://www.aliexpress.com/item/4001111466839.html)
- ST-Link V2 – [Buy Here](https://www.aliexpress.com/item/4001111466839.html)
- Real-Time-Clock DS3231 – [Buy Here](https://www.aliexpress.com/item/4000004876793.html)
- PWM Driver PCA9685 16Ch - [Buy Here](https://www.aliexpress.com/item/32469378576.html)
- 28 x Micro Servos MG90S – [Buy Here](https://www.aliexpress.com/item/32975209678.html?)
- Buck Converter XL4015 5A - [Buy Here](https://www.aliexpress.com/item/32711173698.html)
- 12V Switchng Power Supply - [Buy Here](https://www.aliexpress.com/item/33051556213.html)
- Servo Extension Lead Wire Cable - [Buy Here](https://www.aliexpress.com/item/32827365727.html)

### Machine 
- Acrylic cutting machine
- Stainless Steel cutting machine

### 3D Layout Planning and Modeling

Clock dimension

![80cm dimen](https://user-images.githubusercontent.com/73976219/152270391-0a953df3-c69c-4078-b5cc-d541d93a0d16.PNG)

Final 3D Layout

![Main_Assem1](https://user-images.githubusercontent.com/73976219/152271147-2c590e1f-bcf9-49a3-b3b8-e2b0c246420a.png)

### Circuit Design and PCB

![Screenshot 2022-01-05 221633](https://user-images.githubusercontent.com/73976219/152271374-fc1b3818-85e1-4cf8-a1a8-70fdf87d1227.png)

### Progamming

Modified PCA9685 library from: **Mateusz Salamon** - [Github Link](https://github.com/lamik/Servos_PWM_STM32_HAL)

