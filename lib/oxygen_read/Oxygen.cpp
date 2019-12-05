#include "Oxygen.hpp"

OxygenData_t Oxygen::request(OxygenData_t packet){
    
}

OxygenData_t Oxygen::request(uint8_t command){

}

OxygenData_t Oxygen::request(uint8_t command, uint8_t arg){

}

double Oxygen::sensorValue(OxygenSensorRequest_t sensor){

}

double* Oxygen::allSensorValue(){
    double* result = new double[3];

    return result;
}

bool Oxygen::sensorStatus(){

}

void Oxygen::setOutputMode(OxygenOutputMode_t type){

}

string Oxygen::sensorInfo(OxygenOutputMode_t type){

}