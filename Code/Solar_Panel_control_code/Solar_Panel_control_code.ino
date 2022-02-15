#define direction_signal_M1 2
#define pulse_signal_M1 3
#define on_signal_M1 4

#define direction_signal_M2 5
#define pulse_signal_M2 6
#define on_signal_M2 7

#define len 1

double elevation = 0.0;
double azimuth = 0.0;
int timedata [] = {0,0,0,0,0,0};

int val = 0; 

void setup() {
  
  pinMode(direction_signal_M1, OUTPUT);
  pinMode(pulse_signal_M1, OUTPUT);
  pinMode(on_signal_M1, INPUT);

  pinMode(direction_signal_M2, OUTPUT);
  pinMode(pulse_signal_M2, OUTPUT);
  pinMode(on_signal_M2, INPUT);

}

void loop() {
  
  val = digitalRead(on_signal_M1);
  if(val == HIGH){
    digitalWrite(direction_signal_M1, HIGH);
    delay(1);
    digitalWrite(pulse_signal_M1, HIGH);
    delay(len);
    digitalWrite(pulse_signal_M1, LOW);
    delay(len);  
  }
  if(val == HIGH){
    digitalWrite(direction_signal_M1, LOW);
  }
  
}

void solarzenithelevation(double year, double month, double day, double hour, double minute, double Tgmt){
  double lambdaO = -123.12722630891494 * PI/180;
  double psiO = 49.17491793381123 * PI/180;
  double delta = sundeclination(year, month, day, hour+7, minute);
  double Emin = equationoftime(year, month, day, hour, minute);
  double psiS = delta;
  double lambdaS = -15*(Tgmt - 12 + Emin/60) * PI/180;
  double Sx = cos(psiS)*sin(lambdaS - lambdaO);
  double Sy = cos(psiO) * sin(psiS) - sin(psiO) * cos(psiS) * cos(lambdaS - lambdaO);
  double Sz = sin(psiO) * sin(psiS) + cos(psiO) * cos(psiS) * cos(lambdaS - lambdaO);
  double Z = asin(Sz);
  double ys = atan2(Sx,Sy);

  elevation = Z * 180/PI;
  azimuth = ys * 180/PI;
  if(azimuth < 0.0){
    azimuth += 360;
  }
}

double rangecheck(double value){
  while(value < 0){
    value = value + 360;
  }
  while(value > 360){
    value = value - 360;
  }

  return value;
}

double sundeclination(double year, double month, double day, double hour, double minute){
  double N = Ncalc(year, month, day, hour, minute);
  double declination = asin(sin(-23.44*DEG_TO_RAD) * cos(2*PI/365.24*(N+10) + 2*0.0167 * sin(2*PI/365.24*(N-2))));
  return declination;
}

double greenwichmeantime(int switchstate, int timedata []){


  if(switchstate == 1){
    return timedata[3] + (double)timedata[4]/60 + (double)timedata[5]/(60*60);
  }
  return 0.0;
}

double equationoftime(double year, double month, double day, double hour, double minute){
  double timezone = -7;

  double aaa = 367 * year - 730531.5;
  double bbb = -int((7 * int(year + (month + 9)/12))/4);
  double ccc = int(275 * month/9) + day;
  double Dtoday = (hour + minute/9) + day;
  double Ddays = aaa + bbb + ccc + Dtoday;
  double Cycle = int(Ddays / 365.25);
  double thetarad = 0.0172024 * (Ddays - 365.25 * Cycle);
  double amp1 = 7.36303 - Cycle * 0.00009;
  double amp2 = 9.92465 - Cycle * 0.00014;
  double phi1 = 3.07892 - Cycle * 0.00019;
  double phi2 = -1.38995 + Cycle * 0.00013;
  double EoT1 = amp1 * sin(1 * (thetarad + phi1));
  double EoT2 = amp2 * sin(2 * (thetarad + phi2));
  double EoT3 = 0.31730 * sin(3 * (thetarad - 0.94686));
  double EoT4 = 0.21922 * sin(4 * (thetarad - 0.60716));
  double EoTmins = 0.00526 + EoT1 + EoT2 + EoT3 + EoT4;

  return EoTmins;
}

double Ncalc(int year, double month, double day, double hour, double minute){
  double daysMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  double days = 0.0;

  for(int i=0; i < month; i++){
    double num = year % 4;
    if(num != 0){
      days = days + daysMonth[i];
    }
    
    else{
      if(i == 1){
        days = days + daysMonth[i] + 1;
      }
    }
  }
  return days + day + hour/24 + minute/1440;
}