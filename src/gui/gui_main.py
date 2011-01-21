from PyQt4 import QtCore, QtGui
import sys
import systems
import simulations
import tempfile
import os
import ConfigParser
import subprocess
import re

class LogWindow(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.setWindowTitle("Simulation Log")
        self.resize(300, 200)

        self.controlFrame = QtGui.QFrame(self)

        self.textArea = QtGui.QTextBrowser(self.controlFrame)
        self.textArea.setGeometry(QtCore.QRect(0,0,300,200))

        self.contents = str()

        self.textArea.setText(self.contents)

    def addText(self, text):
        self.contents += text
        self.textArea.setText(self.contents)

class ControlWindow(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.setWindowTitle("System Controls")
        self.resize(300, 200)
    
        self.controlFrame = QtGui.QFrame(self)

        self.systemLabel = QtGui.QLabel(self.controlFrame)
        self.systemLabel.setText("System")
        self.systemLabel.setGeometry(QtCore.QRect(0,0,100,24))
        self.systemComboBox = QtGui.QComboBox(self.controlFrame)
        self.systemComboBox.setGeometry(QtCore.QRect(101,0,200,24))
        for system in sorted(systems.systems.keys()):
            self.systemComboBox.addItem(system)

        self.ruleLabel = QtGui.QLabel(self.controlFrame)
        self.ruleLabel.setText("Rule")
        self.ruleLabel.setGeometry(QtCore.QRect(0,25,100,24))
        self.ruleComboBox = QtGui.QComboBox(self.controlFrame)
        self.ruleComboBox.setGeometry(QtCore.QRect(101,25,200,24))

        self.validatorLabel = QtGui.QLabel(self.controlFrame)
        self.validatorLabel.setText("Validator")
        self.validatorLabel.setGeometry(QtCore.QRect(0,50,100,24))
        self.validatorComboBox = QtGui.QComboBox(self.controlFrame)
        self.validatorComboBox.setGeometry(QtCore.QRect(101,50,200,24))
        
        self.verticalLabel = QtGui.QLabel(self.controlFrame)
        self.verticalLabel.setText("Vertical Axis")
        self.verticalLabel.setGeometry(QtCore.QRect(0,75,100,24))
        self.verticalComboBox = QtGui.QComboBox(self.controlFrame)
        self.verticalComboBox.setGeometry(QtCore.QRect(101,75,200,24))

        self.horizontalLabel = QtGui.QLabel(self.controlFrame)
        self.horizontalLabel.setText("Horizontal Axis")
        self.horizontalLabel.setGeometry(QtCore.QRect(0,100,100,24))
        self.horizontalComboBox = QtGui.QComboBox(self.controlFrame)
        self.horizontalComboBox.setGeometry(QtCore.QRect(101,100,200,24))

        self.timeStepLabel = QtGui.QLabel(self.controlFrame)
        self.timeStepLabel.setText("Time Step")
        self.timeStepLabel.setGeometry(QtCore.QRect(0,125,100,24))
        self.timeStepSpin = QtGui.QDoubleSpinBox(self.controlFrame)
        self.timeStepSpin.setGeometry(QtCore.QRect(101,125,200,24))
        self.timeStepSpin.setMinimum(0.001)
        self.timeStepSpin.setSingleStep(0.001)
        self.timeStepSpin.setDecimals(4)
        self.timeStepSpin.setValue(0.01)

        self.timeLimitLabel = QtGui.QLabel(self.controlFrame)
        self.timeLimitLabel.setText("Time Limit")
        self.timeLimitLabel.setGeometry(QtCore.QRect(0,150,100,24))
        self.timeLimitSpin = QtGui.QDoubleSpinBox(self.controlFrame)
        self.timeLimitSpin.setGeometry(QtCore.QRect(101,150,200,24))
        self.timeLimitSpin.setMaximum(10000)
        self.timeLimitSpin.setDecimals(0)
        self.timeLimitSpin.setValue(100)

        self.runButton = QtGui.QPushButton(self.controlFrame)
        self.runButton.setText("Run")
        self.runButton.setGeometry(QtCore.QRect(100,175,100,24))

        self.writeButton = QtGui.QPushButton(self.controlFrame)
        self.writeButton.setText("To file")
        self.writeButton.setGeometry(QtCore.QRect(0,175,100,24))

        self.submitButton = QtGui.QPushButton(self.controlFrame)
        self.submitButton.setText("To queue")
        self.submitButton.setGeometry(QtCore.QRect(200,175,100,24))
        

    def change_params(self, system):
        self.verticalComboBox.clear()
        self.horizontalComboBox.clear()        
       
        for var in sorted(system.params):
            self.verticalComboBox.addItem(var)
            self.horizontalComboBox.addItem(var)

class ParameterWindow(QtGui.QWidget):
    def __init__(self, system, current_vals, horizontal, vertical):
        QtGui.QWidget.__init__(self)
        self.setWindowTitle("Parameter Controls")
        self.resize(300, 600)
        self.paramFrame = QtGui.QFrame(self)

        self.paramSpins = list()
        self.paramLabels = list()

        position = 0
        for var, default in sorted(zip(system.params, system.defaults)):
            self.paramLabels.append(QtGui.QLabel(self.paramFrame))
            self.paramLabels[-1].setGeometry(QtCore.QRect(0, position, 100, 24))
            self.paramLabels[-1].setText(var)
            
            if var in [horizontal, vertical]:
                key = var + "LimitLower"
                self.paramSpins.append(QtGui.QDoubleSpinBox(self.paramFrame))
                self.paramSpins[-1].setObjectName(key)
                self.paramSpins[-1].setDecimals(8)
                self.paramSpins[-1].setRange(-1000,1000)
                if key in current_vals.keys():
                    self.paramSpins[-1].setValue(current_vals[key])
                else:
                    self.paramSpins[-1].setValue(default)

                self.paramSpins[-1].setGeometry(\
                    QtCore.QRect(100, position, 200, 24))

                position += 25

                key = var + "LimitPixel"
                self.paramSpins.append(QtGui.QSpinBox(self.paramFrame))
                self.paramSpins[-1].setObjectName(key)
##                self.paramSpins[-1].setDecimals(8)
##                self.paramSpins[-1].setMinimum(0.0001)
##                self.paramSpins[-1].setSingleStep(0.01)
##                self.paramSpins[-1].setRange(-1000,1000)
                self.paramSpins[-1].setMinimum(1)
                self.paramSpins[-1].setSingleStep(5)
                self.paramSpins[-1].setRange(1,100000)
                if key in current_vals.keys():
                    self.paramSpins[-1].setValue(current_vals[key])
                else:
                    self.paramSpins[-1].setValue(0.01)

                self.paramSpins[-1].setGeometry(\
                    QtCore.QRect(100, position, 200, 24))
                
                position += 25

                key = var + "LimitUpper"
                self.paramSpins.append(QtGui.QDoubleSpinBox(self.paramFrame))
                self.paramSpins[-1].setObjectName(key)
                self.paramSpins[-1].setDecimals(8)
                self.paramSpins[-1].setRange(-1000,1000)
                if key in current_vals.keys():
                    self.paramSpins[-1].setValue(current_vals[key])
                else:
                    self.paramSpins[-1].setValue(default + 1)
                    
                self.paramSpins[-1].setGeometry(\
                    QtCore.QRect(100, position, 200, 24))
                
                position += 25
            else:
                self.paramSpins.append(QtGui.QDoubleSpinBox(self.paramFrame))
                self.paramSpins[-1].setObjectName(var)
                self.paramSpins[-1].setDecimals(8)
                if var in current_vals.keys():
                    self.paramSpins[-1].setValue(current_vals[var])
                else:
                    self.paramSpins[-1].setValue(default)
                self.paramSpins[-1].setRange(-1000,1000)

                self.paramSpins[-1].setGeometry(\
                    QtCore.QRect(100, position, 200, 24))

                position += 25

class LagrangiansMainWindow(QtGui.QMainWindow):
    def __init__(self):
        #  set up the main frame
        QtGui.QMainWindow.__init__(self)

        self.setWindowTitle("Lagrangians")
        self.resize(800, 600)

        self.menubar = QtGui.QMenuBar()
        self.setMenuBar(self.menubar)

        # Set up the file menu
        self.menuFile = QtGui.QMenu(self.menubar)
        self.menuFile.setTitle("File")
        self.menubar.addAction(self.menuFile.menuAction())

        self.actionOpen = QtGui.QAction(self)
        self.actionOpen.setText("Open")
        self.menuFile.addAction(self.actionOpen)
        self.connect(self.actionOpen, QtCore.SIGNAL("triggered()"), \
                     self.open_file)

        self.actionSave = QtGui.QAction(self)
        self.actionSave.setText("Save")
        self.menuFile.addAction(self.actionSave)
        self.connect(self.actionSave, QtCore.SIGNAL("triggered()"), \
                     self.save_file)

        self.actionExit = QtGui.QAction(self)
        self.actionExit.setText("Exit")
        self.menuFile.addAction(self.actionExit)
        self.connect(self.actionExit, QtCore.SIGNAL("triggered()"),\
                     QtCore.SLOT("close()"))

        # Add a graphics window
        self.centralWidget = QtGui.QWidget(self)
        self.setCentralWidget(self.centralWidget)
        
        self.imageView = QtGui.QLabel(self.centralWidget)
        self.image = QtGui.QImage()
        self.imageView.setPixmap(QtGui.QPixmap.fromImage(self.image))
        self.imageView.setGeometry(QtCore.QRect(0,0,800,600))
        self.imageView.setScaledContents(True)
        self.imageView.setObjectName("imageView")

        # Controls
        self.controlWindow = ControlWindow()
        self.controlWindow.show()
        self.connect(self.controlWindow.systemComboBox, \
                     QtCore.SIGNAL("currentIndexChanged(int)"), \
                     self.change_system)
        self.connect(self.controlWindow.horizontalComboBox, \
                     QtCore.SIGNAL("currentIndexChanged(int)"), \
                     self.new_param_window)
        self.connect(self.controlWindow.verticalComboBox, \
                     QtCore.SIGNAL("currentIndexChanged(int)"), \
                     self.new_param_window)
        self.connect(self.controlWindow.runButton, \
                     QtCore.SIGNAL("clicked()"), \
                     self.do_simulation)
        self.connect(self.controlWindow.writeButton, \
                     QtCore.SIGNAL("clicked()"), \
                     self.write_to_file)
        self.connect(self.controlWindow.submitButton, \
                     QtCore.SIGNAL("clicked()"), \
                     self.submit_to_queue)

        # Initialize settings
        self.change_system()

##        # Log window
##        self.logWindow = LogWindow()
##        self.logWindow.show()

        # Help menu
        self.menuHelp = QtGui.QMenu(self.menubar)
        self.menuHelp.setTitle("Help")
        self.menubar.addAction(self.menuHelp.menuAction())

        self.actionAbout = QtGui.QAction(self)
        self.actionAbout.setText("About")
        self.menuHelp.addAction(self.actionAbout)
        self.connect(self.actionAbout, QtCore.SIGNAL("triggered()"), self.about)      

    def about(self):
        QtGui.QMessageBox.about(self, "About Lagrangians", \
                                "Created by Thomas Bischof.\nVersion 0.1")

    def save_file(self):
        fileDialog = QtGui.QFileDialog(self)
        filename = str(fileDialog.getSaveFileName(self, \
                        "Save input file", os.getcwd(), "Input Files (*.inp)"))
        
        if not filename:
            return(False)
        root, name = os.path.split(filename)
        if root:
            os.chdir(root)
        if not name.endswith(".inp"):
            self.base_filename = name
        else:
            self.base_filename = name[:-4]
        self.set_names()
        return(True)
    
    def open_file(self):
        fileDialog = QtGui.QFileDialog(self)
        self.base_filename = str(fileDialog.getOpenFileName(self, \
                        "Open input file", os.getcwd(), "Input Files (*.inp)"))

        head, tail = os.path.split(self.base_filename)

        os.chdir(head)
        self.base_filename = tail[:-4]
        self.set_names()

        if os.path.isfile(self.image_filename):
            self.update_image()

        self.config_to_window()
            
    def set_names(self):
        if os.path.isfile(self.base_filename + ".ppm"):
            self.image_filename = self.base_filename + ".ppm"
        else:
            self.image_filename = self.base_filename + ".png"
            
        self.input_filename = self.base_filename + ".inp"
        self.restart_filename = self.base_filename + ".restart"
        self.raw_filename = self.base_filename + ".raw"
        self.setWindowTitle(self.base_filename)

    def change_system(self):
        self.system = systems.systems[ \
            str(self.controlWindow.systemComboBox.currentText())]

        self.controlWindow.ruleComboBox.clear()      
        for rule in self.system.rules:
            self.controlWindow.ruleComboBox.addItem(rule)

        self.controlWindow.validatorComboBox.clear()
        self.controlWindow.validatorComboBox.addItem("")
        for validator in self.system.validators:
            self.controlWindow.validatorComboBox.addItem(validator)

        self.controlWindow.change_params(self.system)
        
        self.new_param_window()

    def new_param_window(self):
        current_values = dict()
        try:
            for spinner in self.paramWindow.paramSpins:
                current_values[str(spinner.objectName())] = \
                                                        float(spinner.value())
            self.paramWindow.destroy()
        except:
            pass

        self.paramWindow = ParameterWindow(self.system, current_values,
                        self.controlWindow.horizontalComboBox.currentText(), \
                        self.controlWindow.verticalComboBox.currentText())
        self.paramWindow.show()
        

    def read_config(self):
        config_file = tempfile.TemporaryFile("w+t")
        config_file.write('[config]\n')
        f = open(self.input_filename, "r")
        for line in f:
            config_file.write(line)
        f.close()
        config_file.seek(0)

        parser = ConfigParser.SafeConfigParser()
        parser.readfp(config_file)

        return(parser)

    def config_to_window(self):
        parser = self.read_config()
        system = parser.get("config", "system")
        self.controlWindow.systemComboBox.setCurrentIndex(\
            self.controlWindow.systemComboBox.findText(system))
            
        plot = map(lambda x: x.strip(),parser.get("config", "plot").split(","))
        self.controlWindow.horizontalComboBox.setCurrentIndex(\
            self.controlWindow.horizontalComboBox.findText(plot[0]))
        self.controlWindow.verticalComboBox.setCurrentIndex(\
            self.controlWindow.verticalComboBox.findText(plot[1]))
        
        for spinner in self.paramWindow.paramSpins:
            name = str(spinner.objectName())
            if not "Limit" in name:
                try:
                    val = float(parser.get("config", name))
                    spinner.setValue(val)
                except:
                    pass
            else:
                vals = list(map(float, parser.get("config", \
                                              name[:-10]).split(",")))
                vals[1] = int(vals[1])
                
                if name.endswith("Lower"):
                    spinner.setValue(vals[0])
                elif name.endswith("Pixels"):
                    spinner.setValue(vals[1])
                else:
                    spinner.setValue(vals[2])

        time_vals = list(map(float, parser.get("config", "t").split(",")))
        self.controlWindow.timeStepSpin.setValue(time_vals[1])
        self.controlWindow.timeLimitSpin.setValue(time_vals[2])

        try:
            validator = parser.get("config", "validator")
            if validator:
                self.controlWindow.validatorComboBox.setCurrentIndex(\
                    self.controlWindow.validatorComboBox.findText(validator))
        except:
            pass

        try:
            rule = parser.get("config", "rule")
            if validator:
                self.controlWindow.ruleComboBox.setCurrentIndex(\
                    self.controlWindow.ruleComboBox.findText(rule))
        except:
            pass

    def update_image(self):
        self.image = QtGui.QImage(self.image_filename)
        self.imageView.setPixmap(QtGui.QPixmap.fromImage(self.image))

    def make_input_deck(self):
        self.input_deck = simulations.InputDeck()
        self.input_deck.base_name = self.base_filename

    
        self.input_deck.params["system"] = \
            self.controlWindow.systemComboBox.currentText()
        validator = self.controlWindow.validatorComboBox.currentText()
        if validator:
            self.input_deck.params["validator"] = validator
        self.input_deck.params["rule"] = \
            self.controlWindow.ruleComboBox.currentText()
        
        self.input_deck.params["t"] = ",".join(map(str, [0, \
            float(self.controlWindow.timeStepSpin.value()), \
            float(self.controlWindow.timeLimitSpin.value())]))
        self.input_deck.params["plot"] = ",".join([ \
            str(self.controlWindow.horizontalComboBox.currentText()), \
            str(self.controlWindow.verticalComboBox.currentText())])

        ranges = dict()
        for spinner in self.paramWindow.paramSpins:
            key = str(spinner.objectName())
            if "Limit" in key:
                parser = re.search("(?P<key>.+)Limit(?P<part>.+)", key)
                key = parser.group("key")
                part = parser.group("part")
                if not key in ranges.keys():
                    ranges[key] = [0,0,0]
                if part == "Lower":
                    ranges[key][0] = spinner.value()
                elif part == "Pixel":
                    ranges[key][1] = spinner.value()
                else:
                    ranges[key][2] = spinner.value()
            else:
                self.input_deck.params[key] = spinner.value()

        for key in ranges.keys():
            self.input_deck.params[key] = ranges[key]

    def do_simulation(self):
        if not self.write_to_file():
            return()

        if os.path.isfile(self.restart_filename):
            os.remove(self.restart_filename)
            os.remove(self.raw_filename)
        else:
            pass

        subprocess.Popen(["lagrangians", self.input_filename]).wait()
        self.set_names()
        self.update_image()

    def submit_to_queue(self):
        if not self.write_to_file():
            return()
        
        queue_file = "#!/bin/sh\n"
        queue_file += "cd %s\n" % os.getcwd()
        queue_file += "%s %s\n" % ("lagrangians", self.input_filename)
        queue_file += "convert %s.ppm %s.png\n" % (self.base_filename, \
                                                   self.base_filename)
        queue_file += "bzip2 %s.raw %s.ppm\n" % (self.base_filename, \
                                                   self.base_filename)
        queue_script = open(self.base_filename + ".sh", "w")
        queue_script.write(queue_file)
        queue_script.close()

        subprocess.Popen(["qsub", self.base_filename+".sh"]).wait()

    def write_to_file(self):
        if not self.save_file():
            return(False)
        self.make_input_deck()
        f = open(self.input_filename, "w")
        f.write(str(self.input_deck))
        f.close()
        return(True)

def main(args):
    app = QtGui.QApplication(args)
    lagrangians = LagrangiansMainWindow()
    lagrangians.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main(sys.argv)
    
