package be.eid.eidtestinfra.pcsccontrol.gui;

import java.util.HashMap;
import java.util.Map;

import javax.swing.Action;
import javax.swing.ActionMap;

/**
 * Action map with a setEnabled function to enable or disable all actions in one go.
 * 
 * @author Rogier Taal
 * 
 */
public class DefaultActionMap extends ActionMap {
	private static final long serialVersionUID = 1L;
	
	private Map<Object, Integer> actionEnabledMap = new HashMap<Object, Integer>();
	
	public static final String[] SAVE_PROFILE = new String[] {
		ExitAction.NAME, ToggleActiveFileAction.NAME, SetIncludeDirAction.NAME, ReloadAction.NAME,
		SaveAction.NAME, SetVisibilityAction.NAME, SelectControlFileAction.NAME, ShowAboutAction.NAME,
		CardTreePopupAction.NAME};
	
	public void put(Object key, Action action) {
		super.put(key, action);
		if(action == null) {
			actionEnabledMap.remove(key);
		} else {
			actionEnabledMap.put(key, new Integer(0));
		}
	}
	
	public void remove(Object key) {
		super.remove(key);
		actionEnabledMap.remove(key);
	}
	
	public void clear() {
		super.clear();
		actionEnabledMap.clear();
	}
	
	/**
	 * Enable/disable all actions inside the profile.
	 * @profile
	 * @param enabled
	 */
	public void setEnabled(String[] profile, boolean enabled) {
		for(String name : profile)
			setEnabled(name, enabled);
	}
	
	/**
	 * Enables the given action. The number of times an action is disabled is counted
	 * so that an action only gets enabled when enabled count reaches disabled count.
	 * @param name
	 * @param enabled
	 */
	public void setEnabled(String name, boolean enabled) {
		Integer cnt = actionEnabledMap.get(name);
		if(cnt == null)
			return;
		synchronized (cnt) {
			if(enabled) {
				if(cnt < 0)
					cnt++;
			} else {
				cnt--;
			}
			get(name).setEnabled(cnt == 0);
		}
	}
}
