from enum import Enum
import argparse
import socket
import zeep
import threading
import os

def get_datetime():
    """Función para obtener la fecha y hora del servicio SOAP"""
    try:
        wsdl_url = "http://localhost:8000/?wsdl"
        soap = zeep.Client(wsdl=wsdl_url)
        return soap.service.GetDateTime()
    except Exception as e:
        print("Error obteniendo la hora del servidor SOAP:", e)
        return "00/00/0000 00:00:00"

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    username_activo = "not_connected"
    file_server_port = None

    # ******************** METHODS *******************
    @staticmethod
    def start_file_server(listen_port=0):
        """Función para iniciar el servidor de archivos."""
        def file_server():
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                s.bind(("", listen_port))
                s.listen(5)
                actual_port = s.getsockname()[1]
                print(f"[FileServer] Listening for file requests on port {actual_port}")
                client.file_server_port = actual_port
                while True:
                    conn, addr = s.accept()
                    threading.Thread(target=client.handle_file_request, args=(conn, addr), daemon=True).start()
        threading.Thread(target=file_server, daemon=True).start()

    @staticmethod
    def handle_file_request(conn, addr):
        """Función para manejar las peticiones de archivos."""
        try:
            op = conn.recv(1024).decode().strip()
            if not op.startswith("GET_FILE"):
                conn.send(bytes([2]))
                conn.close()
                return
            file_path = conn.recv(1024).decode().strip()
            if not os.path.isfile(file_path):
                conn.send(bytes([1]))
                conn.close()
                return
            conn.send(bytes([0]))
            file_size = os.path.getsize(file_path)
            conn.sendall(str(file_size).encode() + b"\n")
            with open(file_path, "rb") as f:
                while True:
                    data = f.read(4096)
                    if not data:
                        break
                    conn.sendall(data)
        except Exception as e:
            print(f"[FileServer] Error: {e}")
        finally:
            conn.close()

    @staticmethod
    def  register(user) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"REGISTER {user} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  unregister(user) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if user == client.username_activo:
                print("ESTE USUARIO ESTÁ CONECTADO. DESCONECTA AL USUARIO ANTES DE ELIMINAR")
                return client.RC.ERROR
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"UNREGISTER {user} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def connect(user):
        import time
        # Esperar a que el file server tenga puerto asignado
        while client.file_server_port is None:
            time.sleep(0.1)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if client.username_activo != "not_connected":
                print("YA HAY UN USUARIO CONECTADO")
                return client.RC.ERROR
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            # Enviar el puerto real del file server
            command = f"CONNECT {user} {client.file_server_port} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                client.username_activo = user
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  disconnect(user) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"DISCONNECT {user} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                client.username_activo = "not_connected"
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  publish(fileName,  description) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"PUBLISH {client.username_activo} {fileName} {timestamp} {description}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  delete(fileName) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"DELETE {client.username_activo} {fileName} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  listusers() :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"LIST_USERS  {client.username_activo} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def  listcontent(user) :
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"LIST_CONTENT {client.username_activo} {user} {timestamp}"
            s.send(command.encode())
            response = s.recv(1024).decode()
            code, *message = response.split(" ", 1)
            message = message[0] if message else ""
            print(message)

            code = int(code)
            if code == 0:
                return client.RC.OK
            elif code == 1:
                return client.RC.USER_ERROR

        return client.RC.ERROR


    @staticmethod
    def getfile(user, remote_FileName, local_FileName):
        # Obtener IP y puerto del usuario remoto
        user_ip = None
        user_port = None
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((client._server, client._port))
            timestamp = get_datetime()
            command = f"LIST_USERS {client.username_activo} {timestamp}"
            s.send(command.encode())
            response = s.recv(4096).decode()
            code, *rest = response.split(" ", 1)
            if int(code) != 0:
                print("GET_FILE FAIL: Cannot retrieve user list.")
                return client.RC.ERROR
            tokens = rest[0].split()
            for i in range(0, len(tokens), 3):
                if tokens[i] == user:
                    user_ip = tokens[i+1]
                    user_port = int(tokens[i+2])
                    break
        if not user_ip or not user_port:
            print("GET_FILE FAIL: User not found or not connected.")
            return client.RC.ERROR

        # Conexión al cliente remoto
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(10)  # Timeout de 10 segundos
                s.connect((user_ip, user_port))
                s.sendall(b"GET_FILE\n")
                s.sendall((remote_FileName + "\n").encode())
                
                # Respuesta del cliente remoto
                code = s.recv(1)
                if not code:
                    print("GET_FILE FAIL: No response from remote client.")
                    return client.RC.ERROR
                code = int.from_bytes(code, byteorder='big')

                if code == 0:
                    # Recibir tamaño del archivo
                    size_str = b""
                    while not size_str.endswith(b"\n"):
                        data = s.recv(1)
                        if not data:
                            raise Exception("Connection closed")
                        size_str += data
                    file_size = int(size_str.decode().strip())

                    # Recibir archivo (usamos un archivo temporal)
                    temp_file = local_FileName + ".tmp"
                    received = 0
                    with open(temp_file, "wb") as f:
                        while received < file_size:
                            chunk = s.recv(min(4096, file_size - received))
                            if not chunk:
                                raise Exception("Incomplete transfer")
                            f.write(chunk)
                            received += len(chunk)

                    # Renombrar si la transferencia fue exitosa
                    os.rename(temp_file, local_FileName)
                    print("GET_FILE OK")
                    return client.RC.OK

                elif code == 1:
                    print("GET_FILE FAIL, FILE NOT EXIST")
                    return client.RC.USER_ERROR
                else:
                    print("GET_FILE FAIL")
                    return client.RC.ERROR

        except Exception as e:
            # Borrar archivo temporal si existe
            if os.path.exists(local_FileName + ".tmp"):
                os.remove(local_FileName + ".tmp")
            print(f"GET_FILE FAIL: {str(e)}")
            return client.RC.ERROR

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")

                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;

        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        client.start_file_server()
        #  Write code here
        client.shell()
        print("+++ FINISHED +++")


if __name__=="__main__":
    client.main([])