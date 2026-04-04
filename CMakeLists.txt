cmake_minimum_required(VERSION 3.16)


find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer Xml)


add_subdirectory(xsd)
# add_subdirectory(worker)
add_subdirectory(manager)
