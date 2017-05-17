public interface CNCListener {
	public void onStop();

	public void onForceStop();

	public void onError(String errorMessage);

	public void onArduinoConnected();
}