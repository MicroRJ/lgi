

elBool
lgi_platform__setActiveWindow(HWND hWnd) {
	HWND lastWnd = SetActiveWindow(hWnd);
	return lastWnd != INVALID_HANDLE_VALUE;
}
