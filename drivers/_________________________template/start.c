

void main(void)
{
	// Create driver
	osThreadDef(driver_task_m, 	driver_task, 	osPriorityNormal, 		0, 	1024);
	osThreadCreate (osThread(driver_task_m), NULL);
}