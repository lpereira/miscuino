
class Task {
  private:
    long m_countdown, m_period;
    void (*m_callback) ();
  public:
     Task(void (*callback) (), long period):m_callback(callback),
	m_countdown(period), m_period(period) {

    }
    
    long inline getCountdown(void) {
	return m_countdown;
    }

    long inline getPeriod(void) {
	return m_period;
    }

    void inline setCountdown(long countdown) {
	m_countdown = countdown;
    }

    void inline decrementCountdownBy(long by) {
	m_countdown -= by;
    }

    void inline incrementCountdownBy(long by) {
	m_countdown += by;
    }

    inline void run() {
	m_callback();
    }
};


class TaskManager {
  private:
    Task * m_tasks;
    unsigned int m_nTasks;
    unsigned long m_lastTime;
  public:
     TaskManager(Task * tasks, int n_tasks):m_tasks(tasks),
	m_nTasks(n_tasks), m_lastTime(0L) {

    }
    
    void loop() {
	Task *task;
	unsigned int n_task;
	unsigned long elapsedTime = millis() - m_lastTime;

	for (n_task = 0; n_task < m_nTasks; n_task++) {
	    task = &m_tasks[n_task];

	    task->decrementCountdownBy(elapsedTime);
	    if (task->getCountdown() <= 0) {
		task->run();
		task->incrementCountdownBy(task->getPeriod());
	    }
	}

	m_lastTime = millis();

	asm("sleep");
	delay(1);
    }
};

void hello_task()
{
    static unsigned int n = 0;

    Serial.print("Hello from SimpleTask! (n = ");
    Serial.print(n);
    Serial.println(")");

    n++;
}

void blink_task()
{
    digitalWrite(13, 1);
    delay(20);
    digitalWrite(13, 0);
}

Task tasks[] = { Task(hello_task, 1000), Task(blink_task, 250) };

TaskManager task_manager(tasks, 2);

void setup()
{
    Serial.begin(9600);
    pinMode(13, OUTPUT);
}

void loop()
{
    task_manager.loop();
}
