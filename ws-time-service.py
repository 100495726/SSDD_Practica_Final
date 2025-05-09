from spyne import Application, rpc, ServiceBase, Unicode
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication
from datetime import datetime

class DateTimeService(ServiceBase):
    @rpc(_returns=Unicode)
    def GetDateTime(ctx):
        now = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        return now

app = Application(
    [DateTimeService],
    tns='spyne.datetime.service',
    in_protocol=Soap11(validator='lxml'),
    out_protocol=Soap11()
)

if __name__ == '__main__':
    import logging
    from wsgiref.simple_server import make_server

    logging.basicConfig(level=logging.INFO)
    logging.getLogger('spyne.protocol.soap').setLevel(logging.DEBUG)

    wsgi_app = WsgiApplication(app)
    server = make_server('127.0.0.1', 8000, wsgi_app)
    print("SOAP server running at http://localhost:8000/?wsdl")
    server.serve_forever()
