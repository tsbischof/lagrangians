#!/usr/bin/env python2

from PyQt4 import QtCore, QtGui
import sys
import os
import shutil

class ChoiceWindow(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QWidget.__init__(self)

        self.resize(1000, 1000)
        
        self.images = filter(lambda x: x.endswith(".png"), \
                             sorted(os.listdir(os.getcwd())))
        
        self.like_dir = "like"
        self.dislike_dir = "dislike"
        try:
            os.mkdir(self.like_dir)
        except:
            pass
        
        try:
            os.mkdir(self.dislike_dir)
        except:
            pass

        self.centralWidget = QtGui.QWidget(self)
        self.setCentralWidget(self.centralWidget)

        self.imageView = QtGui.QLabel(self.centralWidget)
        self.imageView.setScaledContents(True)
        self.image = QtGui.QImage()

        self.imageView.setPixmap(QtGui.QPixmap.fromImage(self.image))
        self.imageView.setGeometry(QtCore.QRect(0,0,1000,950))

        self.likeButton = QtGui.QPushButton(self.centralWidget)
        self.likeButton.setText("Like")
        self.likeButton.setGeometry(QtCore.QRect(0,950,500,50))
        self.connect(self.likeButton, QtCore.SIGNAL("clicked()"), self.like)
        self.likeButton.setShortcut(QtGui.QKeySequence("l"))
        
        self.dislikeButton = QtGui.QPushButton(self.centralWidget)
        self.dislikeButton.setText("Dislike")
        self.dislikeButton.setGeometry(QtCore.QRect(500,950,500,50))
        self.connect(self.dislikeButton, QtCore.SIGNAL("clicked()"), \
                     self.dislike)
        self.dislikeButton.setShortcut(QtGui.QKeySequence("d"))

        self.next_image()

    def like(self):
        # move the current image to the like folder, get new image
        shutil.move(self.image_filename, \
               os.path.join(self.like_dir, self.image_filename))
        self.next_image()

    def dislike(self):
        shutil.move(self.image_filename, \
               os.path.join(self.dislike_dir, self.image_filename))
        self.next_image()

    def next_image(self):
        try:
            self.image_filename = self.images.pop()
        except:
            quit()
        print(self.image_filename)
        self.image = QtGui.QImage(self.image_filename)
        self.imageView.setPixmap(QtGui.QPixmap.fromImage(self.image))

        self.setWindowTitle("Swatches - %s" % self.image_filename)

def main(args):
    app = QtGui.QApplication(args)
    window = ChoiceWindow()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main(sys.argv)
