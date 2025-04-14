
// The floats x,y,z should be in decimal amps (i.e. -200mA should be -0.2).
// The value actually sent to the device should be absolute, so if x=-0.2 the current should be set to +0.2 and the negate flag
// should be set.
void initConnection(const char* address, const char* port);
void setAxisCurrent(double x, double y, double z);
void testConnection();
void closeConnection();