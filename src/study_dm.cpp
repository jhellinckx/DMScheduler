#include <utility>
#include <tuple>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <unistd.h>
#include <math.h>

#include "generator.hpp"
#include "task.hpp"
#include "simulator.hpp"
#include "utils.hpp"

const std::string PYTHON_COMMAND = "python";
const std::string PYTHON_PLOTTER_FILENAME = "plotter.py";
const std::size_t SAMPLE_SIZE_PER_VALUE = 10;

const std::string PLOTS_DIR = "plots/";
const std::string PLOT_IMAGE_TYPE = "png";

void stream_double_list(std::stringstream& ss, const std::vector<double>& vec){
	ss << "\"[";
	for(std::size_t i = 0; i < vec.size(); ++i){
		if(isnan(vec[i])) {
			ss << "\'nan\'";
		}
		else{
			ss << vec[i];
		}
		if(i < vec.size() - 1){ ss << ","; }
	}
	ss << "]\"";
}

void prettify_plot(	const std::vector<double>& xs, 
					const std::vector<double>& ys1, const std::vector<double>& ys2, 
					const std::string& x_name, const std::string& y_name, 
					const std::string& y1_label, const std::string& y2_label, 
					const std::string& filename){
	std::stringstream ss;
	ss << PYTHON_COMMAND << " " << PYTHON_PLOTTER_FILENAME << " ";
	stream_double_list(ss, xs);
	ss << " ";
	stream_double_list(ss, ys1);
	ss << " ";
	stream_double_list(ss, ys2);
	ss << " \'" << x_name << "\' \'" << y_name << "\' \'" << y1_label << "\' \'" << y2_label << "\' \'" << PLOTS_DIR << filename << "." << PLOT_IMAGE_TYPE << "\'";
	pid_t pid = fork();
    if(pid < 0){
        throw std::runtime_error("Failed to execute plotting command");
    }
    else if (pid == 0){
        if(system(NULL)){
            system(ss.str().c_str());
            _Exit(EXIT_SUCCESS);
        }
        else{
            _Exit(EXIT_FAILURE);
        }
    }
}



std::tuple<double, double, double, double, double, double, double, double> average_values(Generator gen, unsigned num_procs){
	double tot_load_part = 0.0;
	double tot_load_global = 0.0;
	double schedulable_part = 0;
	double schedulable_global = 0;

	double tot_load_part_min = 0.0;
	double tot_load_global_min = 0.0;
	double schedulable_global_min = 0;
	double schedulable_part_min = 0;

	unsigned required_procs;
	double tot_required_procs_part = 0.0;
	double tot_required_procs_global = 0.0;

	std::vector<Task> tasks;

	for(std::size_t i = 0; i < SAMPLE_SIZE_PER_VALUE; ++i){
		tasks = gen.create_tasks();
		// Fixed procs number
		PDMSimulator p_sim(tasks, num_procs);
		if(p_sim.run()){
			tot_load_part += p_sim.tot_utilization();
			++schedulable_part;
		}
		GDMSimulator g_sim(tasks, num_procs);
		if(g_sim.run()){
			tot_load_global += g_sim.tot_utilization();
			++schedulable_global;
		}
		// Min procs numbers
		required_procs = PDMSimulator::min_partitions(tasks, num_procs);
		PDMSimulator p_sim_min(tasks, required_procs);
		if(p_sim_min.run()){
			tot_load_part_min += p_sim_min.tot_utilization();
			tot_required_procs_part += (double) required_procs;
			++schedulable_part_min;
		}
		required_procs = GDMSimulator::min_procs(tasks, num_procs);
		GDMSimulator g_sim_min(tasks, required_procs);
		if(g_sim_min.run()){
			tot_load_global_min += g_sim_min.tot_utilization();
			tot_required_procs_global += (double) required_procs;
			++schedulable_global_min;
		}
	}
	return std::make_tuple(	tot_load_part / schedulable_part, tot_load_global / schedulable_global,
							tot_load_part_min / schedulable_part_min, tot_load_global_min / schedulable_global_min,
							tot_required_procs_part / schedulable_part_min, tot_required_procs_global / schedulable_global_min,
							schedulable_part / (double) SAMPLE_SIZE_PER_VALUE, schedulable_global / (double) SAMPLE_SIZE_PER_VALUE);
}

void compare_util(){
	// Compare load, required number of procs and schedulability depending on the utilization with fixed and minimum proc numbers and the strategy
	const int num_tasks = 8;
	const unsigned num_procs = 4;
	const double upper_u = 300;
	const double lower_u = 5;
	const double u_step = 5;

	std::size_t data_size = (std::size_t)((upper_u - lower_u) / u_step) + 1;
	std::vector<double> part_loads(data_size);
	std::vector<double> global_loads(data_size);
	std::vector<double> part_loads_min(data_size);
	std::vector<double> global_loads_min(data_size);
	std::vector<double> part_num_required_procs(data_size);
	std::vector<double> global_num_required_procs(data_size);
	std::vector<double> part_num_schedulable(data_size);
	std::vector<double> global_num_schedulable(data_size);
	std::vector<double> utils(data_size);

	std::size_t i = 0;
	for(double u = lower_u; u <= upper_u; u += u_step){
		std::tie(part_loads[i], global_loads[i], part_loads_min[i], global_loads_min[i], part_num_required_procs[i], global_num_required_procs[i], part_num_schedulable[i], global_num_schedulable[i]) = average_values(Generator(u, num_tasks), num_procs);
		utils[i] = u;
		++i;
	}

	prettify_plot(utils, part_loads, global_loads, "utilization", "system load", "partition", "global", "load_util_fixed");
	prettify_plot(utils, part_loads_min, global_loads_min, "utilization", "system load", "partition", "global", "load_util_min");
	prettify_plot(utils, part_num_required_procs, global_num_required_procs, "utilization", "processors required", "partition", "global", "procs_util");
	prettify_plot(utils, part_num_schedulable, global_num_schedulable, "utilization", "schedulable", "partition", "global", "schedulable_util");
}

void compare_num_tasks(){
	// Compare load, required number of procs and schedulability depending on the number of tasks with fixed and minimum proc numbers and the strategy
	const double utilization = 200;
	const unsigned num_procs = 4;
	const unsigned lower_num_tasks = 2;
	const unsigned upper_num_tasks = 30;
	const double n_step = 1;

	std::size_t data_size = (std::size_t)((upper_num_tasks - lower_num_tasks) / n_step) + 1;
	std::vector<double> part_loads(data_size);
	std::vector<double> global_loads(data_size);
	std::vector<double> part_loads_min(data_size);
	std::vector<double> global_loads_min(data_size);
	std::vector<double> part_num_required_procs(data_size);
	std::vector<double> global_num_required_procs(data_size);
	std::vector<double> part_num_schedulable(data_size);
	std::vector<double> global_num_schedulable(data_size);
	std::vector<double> num_tasks(data_size);

	std::size_t i = 0;
	for(double n = lower_num_tasks; n <= upper_num_tasks; n += n_step){
		std::tie(part_loads[i], global_loads[i], part_loads_min[i], global_loads_min[i], part_num_required_procs[i], global_num_required_procs[i], part_num_schedulable[i], global_num_schedulable[i]) = average_values(Generator(utilization, n), num_procs);
		num_tasks[i] = n;
		++i;
	}

	prettify_plot(num_tasks, part_loads, global_loads, "number of tasks", "system load", "partition", "global", "load_num_tasks_fixed");
	prettify_plot(num_tasks, part_loads_min, global_loads_min, "number of tasks", "system load", "partition", "global", "load_num_tasks_min");
	prettify_plot(num_tasks, part_num_required_procs, global_num_required_procs, "number of tasks", "processors required", "partition", "global", "procs_num_tasks");
	prettify_plot(num_tasks, part_num_schedulable, global_num_schedulable, "number of tasks", "schedulable", "partition", "global", "schedulable_num_tasks");
}

int main(){
	compare_util();
	compare_num_tasks();
}
