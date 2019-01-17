from fbs_runtime.application_context import ApplicationContext
from PyQt5.QtWidgets import QMainWindow

import sys, os
from openni import openni2

class AppContext(ApplicationContext):           # 1. Subclass ApplicationContext
	def run(self):                              # 2. Implement run()
		path = os.path.dirname(appctxt.get_resource('libOpenNI2.dylib'))
		openni2.initialize(path)
		window = QMainWindow()
		window.setWindowTitle("Portable Motion Analysis")
		window.resize(250, 150)
		window.show()
		return self.app.exec_()                 # 3. End run() with this line

if __name__ == '__main__':
	appctxt = AppContext()                      # 4. Instantiate the subclass
	exit_code = appctxt.run()                   # 5. Invoke run()
	sys.exit(exit_code)