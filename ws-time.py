import zeep

def get_datetime():
    wsdl_url = "http://localhost:8000/?wsdl"
    soap = zeep.Client(wsdl=wsdl_url)
    result = soap.service.GetDateTime()
    return result

def main():
    timestamp = get_datetime()
    print("Fecha y hora desde servicio SOAP:", timestamp)

if __name__ == '__main__':
    main()
