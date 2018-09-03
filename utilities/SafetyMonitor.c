#include <utilities/SafetyMonitor.h>

void SafeMon_Init(ClassSafetyMonitor * safeMon) {

	safeMon->_varNum = 0;
}

void SafeMon_AddVar(ClassSafetyMonitor * safeMon, float * var, float highBound,
		float lowBound, char varName) {

	if (safeMon->_varNum <= 0) {

		safeMon->_varNum = 0;

		// Allocate memory for one ctlHandler
		safeMon->_varList = (float **) malloc(sizeof(float*));

		safeMon->_highBound = (float *) malloc(sizeof(float));

		safeMon->_lowBound = (float *) malloc(sizeof(float));

		safeMon->_varName = (char *) malloc(sizeof(char));

	} else {

		// ReAllocate memory for the comHandler
		safeMon->_varList = (float**) realloc(safeMon->_varList,
				(safeMon->_varNum + 1) * sizeof(float*));

		safeMon->_highBound = (float*) realloc(safeMon->_highBound,
				(safeMon->_varNum + 1) * sizeof(float));

		safeMon->_lowBound = (float*) realloc(safeMon->_lowBound,
				(safeMon->_varNum + 1) * sizeof(float));

		safeMon->_varName = (char*) realloc(safeMon->_varName,
				(safeMon->_varNum + 1) * sizeof(char));
	}

	safeMon->_varList[safeMon->_varNum] = var;

	safeMon->_highBound[safeMon->_varNum] = highBound;

	safeMon->_lowBound[safeMon->_varNum] = lowBound;

	safeMon->_varName[safeMon->_varNum] = varName;

	safeMon->_varNum++;

}

void SafeMon_Clear(ClassSafetyMonitor * safeMon) {

	free(safeMon->_varList);
	free(safeMon->_highBound);
	free(safeMon->_lowBound);
	free(safeMon->_varName);

	safeMon->_varNum = 0;

}

/**
 * Checks if all the monitored variables are within the specified bounds
 * @param safeMon pointer to the safe monitoring class
 * @return	/0 when all monitored variables are within the specified bounds
 * 			i + 1 where i is the index of the variable which exceded its upper bound
 *			- (i + 1) where i is the index of the variable which exceded its lower bound
 */
uint32_t SafeMon_Check(ClassSafetyMonitor * safeMon) {

	uint32_t i = 0;

	float var2Check;

	for (i = 0; i < safeMon->_varNum; ++i) {

		var2Check = *(safeMon->_varList[i]);

		if (var2Check < safeMon->_lowBound[i])
			return -(i + 1);

		if (var2Check > safeMon->_highBound[i])
			return i + 1;
	}

	return 0;
}

