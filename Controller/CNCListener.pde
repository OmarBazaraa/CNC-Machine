public interface CNCListener {
	public void onStop();

	public void onError(String errorMessage);

	public void onArduinoConnected();
}