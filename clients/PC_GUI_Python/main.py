
import pyforms
from   pyforms          import BaseWidget
from   pyforms.Controls import ControlText
from   pyforms.Controls import ControlButton
from   pyforms.Controls import ControlSlider

import socket
import sys, logging
import Proto


class DollyGui(BaseWidget):

    TCP_IP          = '127.0.0.1'
    TCP_PORT        = 8081
    HOST            = TCP_IP + ":" + str(TCP_PORT)
    BUFFER_SIZE     = 1024
    sock = None

    def __init__(self):
        super(DollyGui,self).__init__('Dolly PC GUI')
        # Define the organization of the forms
        self._formset = [('_server', '_ConnButton', '_DisConnButton'),
                            ' ',
                            ('_ForwardButton', '_FwdSteps', ' '),
                            ('_RewindButton', '_RwdSteps', ' '),
                            ' ',
                            ('_RunButton', '_HomeButton'),
                            ' ',
                            '_QuitButton']
        self.showNormal()

        #Definition of the forms fields
        self._server    = ControlText('Server', self.HOST, "Type in the name host and port for the server\n e.g.: localhost:8080")
        self._FwdSteps  = ControlSlider ('Steps')
        self._FwdSteps.value=1
        self._FwdSteps.min=0
        self._FwdSteps.max=100
        self._RwdSteps = ControlSlider('Steps')
        self._RwdSteps.value = 1
        self._RwdSteps.min = 0
        self._RwdSteps.max = 100

        # Define the button action
        self._ConnButton = ControlButton("Connect")
        self._DisConnButton = ControlButton("Disconnect")
        self._ForwardButton = ControlButton("Forward")
        self._RewindButton = ControlButton("Rewind")
        self._QuitButton = ControlButton("Quit")

        self._RunButton = ControlButton("Run")
        self._HomeButton = ControlButton("Home")
        self._QuitButton = ControlButton("Quit")

        self._ConnButton.value = self.__connBtnAction
        self._DisConnButton.value = self.__DisConnBtnAction
        self._server.changed = self.__updateHost
        self._ForwardButton.value = self.__FwdBtnAction
        self._RewindButton.value = self.__RwdBtnAction
        self._QuitButton.value = self.__QuitBtnAction
        self._DeActivateUi()

    def __QuitBtnAction(self):
        self.__DisConnAction()
        sys.stdout.flush()
        sys.exit(0)

    def __updateHost(self):
        if self.HOST != self._server.value:
            self.HOST = self._server.value
            self.TCP_PORT = int(self.HOST.split(":")[1])
            self.TCP_IP = socket.gethostbyname(self.HOST.split(":")[0])
            print "New host: " + self.TCP_IP + ":" + str(self.TCP_PORT) + " " + self.HOST.split(":")[0]
            self.__DisConnAction()

    def __connBtnAction(self):
        print "connecting to " + self.HOST
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if 0 == self.sock.connect_ex((self.TCP_IP, self.TCP_PORT)):
            self._activateUi()

        else:
            print "Could not connect to " + self.HOST

    def _activateUi(self):
        self._ConnButton.hide()
        self._DisConnButton.show()
        self._ForwardButton.enabled = True
        self._RewindButton.enabled = True
        self._FwdSteps.enabled = True
        self._RwdSteps.enabled = True

    def _DeActivateUi(self):
        self._ConnButton.show()
        self._DisConnButton.hide()
        self._ForwardButton.enabled = False
        self._RewindButton.enabled = False
        self._FwdSteps.enabled = False
        self._RwdSteps.enabled = False

    def __DisConnBtnAction(self):
        self.__DisConnAction()


    def __DisConnAction(self):
        if self.sock != None:
            self.sock.close()
        self._DeActivateUi()
        print "disconnecting"
        self.sock = None

    def __FwdBtnAction(self):
        try:
            msg = Proto.Message()
            msg.type = Proto.FORWARD
            msg.value = str(self._FwdSteps.value)
            msg.length = len(str(msg.value))
            print "Encoded String: " + msg.Serialize
            self.sock.send(msg.Serialize)
            data = self.sock.recv(self.BUFFER_SIZE)
            print "received data:", data
        finally:
            print"nothing"

    def __RwdBtnAction(self):
        try:
            msg = Proto.Message()
            msg.type = Proto.REWIND
            msg.value = self._RwdSteps.value
            msg.length = len(str(msg.value))

            print "Encoded String: " + msg.Serialize
            self.sock.send(msg.Serialize)
            data = self.sock.recv(self.BUFFER_SIZE)
            print "received data:", data
        finally:
            print"nothing"

def log_uncaught_exceptions(exception_type, exception, tb):
    logging.critical(''.join(traceback.format_tb(tb)))
    logging.critical('{0}: {1}'.format(exception_type, exception))

#Execute the application
if __name__ == "__main__":
    sys.excepthook = log_uncaught_exceptions
    pyforms.startApp(DollyGui, geometry=(200, 200, 400, 400))
