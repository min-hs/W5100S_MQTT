/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wizchip_conf.h"
#include "MQTTClient.h"
//#include "dns.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PUBLISH_MYSELF 1

uint8_t bLoopback = 1;
uint8_t bRandomPacket = 0;
uint8_t bAnyPacket = 0;
uint16_t pack_size = 0;

const uint8_t URL[] = "test.mosquitto.org";
uint8_t dns_server_ip[4] = {168,126,63,1};
uint8_t dnsclient_ip[4] = {0,};
unsigned int targetPort = 1883; // mqtt server port
unsigned char targetIP[4] = {0x00, 0x00, 0x00, 0x00}; // mqtt server IP

uint8_t IP_TYPE;
unsigned char destip[4] = {192, 168, 1, 3}; //PC IP addr

const uint8_t MQTT_TOPIC[]="W5x00";
unsigned char tempBuffer[1024*2] = {0, };

void print_network_information(void);
void delay(unsigned int count);
void MQTT_run(void);
void messageArrived(MessageData* md);

struct opts_struct
{
	char *clientid;
	int nodelimiter;
	char *delimiter;
	enum QoS qos;
	char *username;
	char *password;
	char *host;
	int port;
	int showtopics;
} opts =
{
		(char*)"stdout-subscriber",
		0,
		(char*)"\n",
		QOS0,
		"w5100s",
		"0123456789",
		(char *)"192.168.1.10",
		1883,
		0
};

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
wiz_NetInfo defaultNetInfo = { .mac = {0x00,0x08,0xdc,0xff,0xee,0xdd},
							.ip = {192,168,1,130},
							.sn = {255,255,255,0},
							.gw = {192,168,1,1},
							.dns = {168, 126, 63, 1},
							//.dns = {8, 8, 8, 8},
							.dhcp = NETINFO_STATIC};
uint8_t ethBuf0[2048];
uint8_t ethBuf1[2048];
uint8_t ethBuf2[2048];
uint8_t ethBuf3[2048];

volatile unsigned long globalTimer = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void print_network_information(void);
int _write(int fd, char *str, int len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void TIM2_settimer(void)
{
	TIM_Cmd(TIM2,DISABLE);
	globalTimer = 0;
	TIM_Cmd(TIM2,ENABLE);
}

unsigned long TIM2_gettimer(void)
{
	return globalTimer;
}

void csEnable(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

void csDisable(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void spiWriteByte(uint8_t tx)
{
	uint8_t rx;
	HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, 10);
}

uint8_t spiReadByte(void)
{
	uint8_t rx = 0, tx = 0xFF;
	HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, 10);
	return rx;
}
void W5100s_Initialze(void)
{
	csDisable();
	reg_wizchip_cs_cbfunc(csEnable, csDisable);
	reg_wizchip_spi_cbfunc(spiReadByte,spiWriteByte);


#if _WIZCHIP_ == W5100
  HAL_Delay(3000);
#endif
#if _WIZCHIP_ == W5100S
  printf("version:%2x\r\n", getVER());
  printf("TMSR:%2x\r\n", getTMSR());
  //NETUNLOCK();
#endif
	uint8_t tmp;
	//w5500, w5200
#if _WIZCHIP_SOCK_NUM_ == 8
	uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
#else
	uint8_t memsize[2][4] = { {2,2,2,2},{2,2,2,2}};
#endif
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		//myprintf("WIZCHIP Initialized fail.\r\n");
		printf("WIZCHIP Initialized fail.\r\n", 1, 10);
	  return;
	}
	/* PHY link status check */
	do {
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
		{
			printf("Unknown PHY Link status.\r\n", 1, 10);
		  return;
		}
	} while (tmp == PHY_LINK_OFF);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	  printf("\t - W5100S Project - \r\n");
	  resetAssert();
	  HAL_Delay(1000);
	  resetDeassert();
	  HAL_Delay(1000);


  //WIZCHIPInitialize();
  W5100s_Initialze();
  printf("\t - WizChip Init - \r\n");

  wizchip_setnetinfo(&defaultNetInfo);
  print_network_information();

  DNS_init(0, ethBuf0);
  IP_TYPE = 0x1c;
  while (DNS_run(dns_server_ip, URL, dnsclient_ip) != 1);
//  NewNetwork(&g_network, mqtt_socket);
//  ConnectNetwork(&g_network, g_target_ip, MQTT_PORT);
  MQTT_run();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 64000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void print_network_information(void)
{
	wizchip_getnetinfo(&defaultNetInfo);
	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",defaultNetInfo.mac[0],defaultNetInfo.mac[1],defaultNetInfo.mac[2],defaultNetInfo.mac[3],defaultNetInfo.mac[4],defaultNetInfo.mac[5]);
	printf("IP address : %d.%d.%d.%d\n\r",defaultNetInfo.ip[0],defaultNetInfo.ip[1],defaultNetInfo.ip[2],defaultNetInfo.ip[3]);
	printf("SM Mask	   : %d.%d.%d.%d\n\r",defaultNetInfo.sn[0],defaultNetInfo.sn[1],defaultNetInfo.sn[2],defaultNetInfo.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r",defaultNetInfo.gw[0],defaultNetInfo.gw[1],defaultNetInfo.gw[2],defaultNetInfo.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r",defaultNetInfo.dns[0],defaultNetInfo.dns[1],defaultNetInfo.dns[2],defaultNetInfo.dns[3]);
}

int _write(int fd, char *str, int len)
{
	for(int i=0; i<len; i++)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)&str[i], 1, 0xFFFF);
	}
	return len;
}
void MQTT_run(void){

	int rc = 0;
	unsigned char buf[2048] = {0,};
	unsigned char pubbuf[2048] = {0,};
	Network n;
	MQTTClient c;
	MQTTMessage m;
	uint32_t ck_timer;

	NewNetwork(&n, 2);
	printf("newNet Done\r\n");
	ConnectNetwork(&n, destip, 1883);
	printf("conNet Done\r\n");

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 4;
	data.clientID.cstring = opts.clientid;
	data.username.cstring = opts.username;
	data.password.cstring = opts.password;

	data.keepAliveInterval = 60;
	data.cleansession = 1;

	MQTTClientInit(&c, &n, 1000, buf, 2048, pubbuf, sizeof(pubbuf));

	printf("[DEBUG] Subscribe : %s\r\n", MQTT_TOPIC);

	rc = MQTTConnect(&c, &data);
	printf("Connected %d\r\n", rc);
	opts.showtopics = 1;

	printf("Subscribing to %s\r\n", MQTT_TOPIC);
	rc = MQTTSubscribe(&c, MQTT_TOPIC, opts.qos, messageArrived);
	printf("Subscribed %d\r\n", rc);

#if PUBLISH_MYSELF == 1
	m.qos = QOS0;
	m.retained = 0;
	m.dup = 0;

	ck_timer = TIM2_gettimer();
#endif

	while(1)
	{
		MQTTYield(&c, data.keepAliveInterval);

#if PUBLISH_MYSELF == 1
		if(ck_timer + 10000 < TIM2_gettimer())
		{
			ck_timer = TIM2_gettimer();

			//printf("Publishing to %s\r\n", MQTT_TOPIC);

			sprintf(pubbuf, "Hello, W5x00!" );
			m.payload = pubbuf;
			m.payloadlen = strlen(pubbuf);

			rc = MQTTPublish(&c, MQTT_TOPIC, &m);
			printf("Published %d\r\n", rc);
		}
#endif
	}
}

void messageArrived(MessageData* md)
{
	unsigned char testbuffer[100];
	uint32_t rd_size;
	uint32_t rd_pt;
	uint32_t rdmore_size;

	MQTTMessage* message = md->message;
	MQTTString* topic = md->topicName;

	rdmore_size = (int)topic->lenstring.len;
	rd_pt = 0;

	while(rdmore_size != 0)
	{
		if(rdmore_size > sizeof(testbuffer) - 1)
		{
			rd_size = sizeof(testbuffer) - 1;
		}
		else
		{
			rd_size = rdmore_size;
		}

		memset(testbuffer, 0, rd_size + 1);

		memcpy(testbuffer, ((char*)topic->lenstring.data) + rd_pt, rd_size);
		printf("SubScribe Topic : %s\r\n", testbuffer);

		rd_pt += rd_size;
		rdmore_size -= rd_size;
	}

	if (opts.nodelimiter)
	{
		printf("Message : %.*s\r\n", (int)message->payloadlen, (char*)message->payload);
	}
	else
	{
		printf("Message : %.*s%s\r\n", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
