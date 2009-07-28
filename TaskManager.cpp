/*
 * Escalonador para Arduino
 * Versao 1
 *
 * Copyright (c) 2009 Leandro A. F. Pereira <leandro@hardinfo.org>
 * Licenciado sob a GNU GPL versao 2.
 *
 */
 
class Task {
private:
  long m_countdown, m_period;
  void (*m_callback)();
public:
  Task(void (*callback)(), long period) : m_callback(callback), m_countdown(period), m_period(period) {

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

void hello_task() {
    static unsigned int n = 0;
    
    Serial.print("Hello from SimpleTask! (n = ");
    Serial.print(n);
    Serial.println(")");
    
    n++;
}

void blink_task() {
    static unsigned char state = false;
    static unsigned long count = 0;
    
    digitalWrite(13, state);
    Serial.println(count);
    
    count++;
    state = !state;
}

class TaskManager {
private:
  Task *m_tasks;
  unsigned int m_nTasks;
  unsigned long m_lastTime;

public:
  TaskManager(Task *tasks, int n_tasks) : m_tasks(tasks), m_nTasks(n_tasks), m_lastTime(0L) {

  }

  void loop() {
    Task *task;
    unsigned int n_task;
    unsigned long elapsedTime = millis() - m_lastTime;

    for (n_task = 0; n_task < m_nTasks; n_task++) {
      task = &m_tasks[n_task];

      task->decrementCountdownBy(elapsedTime);
    }

    for (n_task = 0; n_task < m_nTasks; n_task++) {
      task = &m_tasks[n_task];

      if (task->getCountdown() <= 0) {
        task->run();
        task->incrementCountdownBy(task->getPeriod());
      }
    }

    m_lastTime = millis();
  }
};

Task tasks[] = { Task(hello_task, 1000), Task(blink_task, 100) };
TaskManager task_manager(tasks, 2);

void setup() {
  Serial.begin(9600);
}

void loop() {
  task_manager.loop();
}
