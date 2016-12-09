import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math


executions = 	[
					[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
					[2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2]
				]

PROC_MARGIN = 0.2
HEIGHT_BASE = 1
WIDTH_BASE = 1
TIME_UNITS_PER_LINE = 50;

IDLE_EXEC = -1

PROC_SYMBOL = "\pi"
TASK_SYMBOL = "\\tau"

OUTPUT_FILENAME = "schedule.png"
DPI = 300

ANNOTATION_FONT_COLOR = "black"
ANNOTATION_FONT_SIZE = 6
ANNOTATION_FONT_WEIGHT = "normal"

TIME_UNIT_FONT_SIZE = 4
EXEC_LINE_WIDTH = 0.3

class GraphicalSchedule(object):
	def __init__(self, executions):
		self.executions = executions
		self.num_procs = len(self.executions)
		self.line_height = self.num_procs * (HEIGHT_BASE + PROC_MARGIN) + HEIGHT_BASE
		self.needed_lines = int(math.ceil(len(max(self.executions, key = lambda x: len(x))) / float(TIME_UNITS_PER_LINE)))
		self.figure = plt.figure()
		self.ax = self.figure.add_subplot(111, aspect = "auto")
		self.ax.axis("off")
		self.tot_height = self.line_height * self.needed_lines
		self.tot_width = (TIME_UNITS_PER_LINE * WIDTH_BASE) + 2 * WIDTH_BASE
		self.ax.set_xlim([0, self.tot_width])
		self.ax.set_ylim([0, self.tot_height])
		self.current_line = 1

	def draw(self):
		for i in range(self.needed_lines):
			self.draw_line()
		self.save()

	def line_starting_time(self):
		return TIME_UNITS_PER_LINE * (self.current_line - 1)

	def draw_line(self):
		height_offset = self.tot_height - (self.current_line * self.line_height)
		self.draw_time_units_textboxes(height_offset)
		height_offset += HEIGHT_BASE
		for i in range (self.num_procs, 0, -1):
			self.draw_proc_textbox(i, height_offset)
			self.draw_proc_schedule(i - 1, height_offset)
			height_offset += HEIGHT_BASE + PROC_MARGIN
		self.current_line += 1

	def draw_time_units_textboxes(self, height_offset):
		for i in range(TIME_UNITS_PER_LINE):
			self.centered_textbox(WIDTH_BASE * i + WIDTH_BASE, height_offset, str(self.line_starting_time() + i), fontsize = TIME_UNIT_FONT_SIZE)

	def draw_proc_textbox(self, proc, height_offset):
		self.centered_textbox(0, height_offset, "$" + PROC_SYMBOL + "_" + str(proc) + "$")

	def draw_proc_schedule(self, proc, height_offset):
		line_time = self.line_starting_time()
		prev_task_id = IDLE_EXEC
		task_id = IDLE_EXEC
		task_execs = 1
		i = 0
		for i in range(min(TIME_UNITS_PER_LINE, len(self.executions[proc]) - line_time)):
			t = i + line_time
			task_id = self.executions[proc][t]
			if task_id == prev_task_id:
				task_execs += 1
			else:
				if prev_task_id != IDLE_EXEC:
					exec_rectangle = patches.Rectangle(((i - task_execs) * WIDTH_BASE + WIDTH_BASE, height_offset), task_execs * WIDTH_BASE, HEIGHT_BASE, fill = False, lw = EXEC_LINE_WIDTH)
					self.ax.add_patch(exec_rectangle)
					self.annotate_rectangle(exec_rectangle, "$" + TASK_SYMBOL + "_" + str(prev_task_id) + "$")
				task_execs = 1
			prev_task_id = task_id
		if task_id != IDLE_EXEC:
			exec_rectangle = patches.Rectangle(((i - task_execs) * WIDTH_BASE + 2 * WIDTH_BASE, height_offset), task_execs * WIDTH_BASE, HEIGHT_BASE, fill = False, lw = EXEC_LINE_WIDTH)
			self.ax.add_patch(exec_rectangle)
			self.annotate_rectangle(exec_rectangle, "$" + TASK_SYMBOL + "_" + str(task_id) + "$")


	def annotate_rectangle(self, r, text, **kwargs):
		rx, ry = r.get_xy()
		cx = rx + r.get_width() / 2.0
		cy = ry + r.get_height() / 2.0
		default_args = {"color" : ANNOTATION_FONT_COLOR, "weight" : ANNOTATION_FONT_WEIGHT, 
			"fontsize" : ANNOTATION_FONT_SIZE, "ha" : 'center', "va" : 'center'}
		self.ax.annotate(text, (cx, cy), **dict(default_args, **kwargs))

	def centered_textbox(self, x, y, text, **kwargs):
		r = patches.Rectangle((x, y), WIDTH_BASE, HEIGHT_BASE)
		self.annotate_rectangle(r, text, **kwargs)

	def save(self):
		plt.savefig(OUTPUT_FILENAME, dpi = DPI, bbox_inches = "tight")

GraphicalSchedule(executions).draw()
