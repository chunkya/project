#include "simdag/simdag.h"
#include "xbt.h"
#include "xbt/log.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(sd_typed_tasks_test, "SimDAG simple");


int main(int argc, char **argv) {
  unsigned int count;
  int total_workstations = 0;
  const SD_workstation_t *workstations = NULL;
  SD_task_t start_task, task, end_task;
  SD_task_t starttransfer, endtransfer;
  xbt_dynar_t changed_tasks;
  
  SD_init(&argc, argv);
  SD_create_environment(argv[1]);

  total_workstations = SD_workstation_get_number();
  workstations = SD_workstation_get_list();


  start_task = SD_task_create_comp_seq("starttask", NULL, 0);
  starttransfer = SD_task_create_comm_e2e("starttransfer", NULL, 1e10);
  task = SD_task_create_comp_seq("task", NULL, 1e9);
  endtransfer = SD_task_create_comm_e2e("endtransfer", NULL, 1e10);
  end_task = SD_task_create_comp_seq("endtask", NULL, 0);
  
  SD_task_dependency_add(NULL, NULL, start_task, starttransfer);
  SD_task_dependency_add(NULL, NULL, starttransfer, task);
  SD_task_dependency_add(NULL, NULL, task, endtransfer);
  SD_task_dependency_add(NULL, NULL, endtransfer, end_task);
  
  SD_task_schedulel(start_task, 1, SD_workstation_get_by_name("mainCPU"));
  SD_task_schedulel(task, 1, SD_workstation_get_by_name("CPU1"));
  SD_task_schedulel(end_task, 1, SD_workstation_get_by_name("mainCPU"));

  while (!xbt_dynar_is_empty((changed_tasks = SD_simulate(-1.0)))) {    
    XBT_INFO("link1: bw=%.0f, lat=%f",
             SD_route_get_current_bandwidth(workstations[0], workstations[1]),
             SD_route_get_current_latency(workstations[0], workstations[1]));
    XBT_INFO("Jupiter: power=%.0f",
             SD_workstation_get_power(workstations[0])*
             SD_workstation_get_available_power(workstations[0]));
    XBT_INFO("Tremblay: power=%.0f",
             SD_workstation_get_power(workstations[1])*
             SD_workstation_get_available_power(workstations[1]));
    xbt_dynar_foreach(changed_tasks, count, task) {
      XBT_INFO("Task '%s' start time: %f, finish time: %f",
           SD_task_get_name(task),
           SD_task_get_start_time(task), SD_task_get_finish_time(task));
      if (SD_task_get_state(task)==SD_DONE)
        SD_task_destroy(task);
    }
    xbt_dynar_free_container(&changed_tasks);
  }
  xbt_dynar_free_container(&changed_tasks);
  SD_exit();
  return 0;
}
