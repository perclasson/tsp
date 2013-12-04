import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class TwoOpt {

	private int[][] distances;

	public TwoOpt(int[][] distances) {
		this.distances = distances;
	}

	public Short[] run(Short[] route, long deadline) {
		boolean improved = true;
		search: while (improved) {
			improved = false;
			for (int i = 1; i < route.length - 1; i++) {
				for (int k = i + 1; k < route.length; k++) {
					if (System.currentTimeMillis() >= deadline)
						break search;
					if (isGain(route, i, k)) {
						twoOptSwap(route, i, k + 1);
						improved = true;
						continue search;
					}
				}
			}
			break;
		}
		return route;
	}

	private boolean isGain(Short[] existingRoute, int i, int k) {
		int a = i - 1;
		int b = i;
		int c = k;
		int d = k + 1;

		if (b == 0)
			a = existingRoute.length - 1;

		if (d == existingRoute.length)
			d = 0;

		int prevCost = distances[existingRoute[a]][existingRoute[b]] + distances[existingRoute[c]][existingRoute[d]];
		int newCost = distances[existingRoute[a]][existingRoute[c]] + distances[existingRoute[b]][existingRoute[d]];

		return (prevCost - newCost) > 0;
	}

	private void twoOptSwap(Short[] route, int i, int k) {
		List<Short> routeList = Arrays.asList(route);
		Collections.reverse(routeList.subList(i, k));
	}

	private void twoOptSwapFail(Short[] route, int i, int k) {
		List<Short> routeList = Arrays.asList(route);
		if ((k - i) < (route.length - 1 - k + i)) {
			Collections.reverse(routeList.subList(i, k));
		} else {
			List<Short> first = new ArrayList<Short>();
			List<Short> last = new ArrayList<Short>();
			if (i > 0)
				first.addAll(routeList.subList(0, i));
			if (k + 1 < route.length)
				last.addAll(routeList.subList(k + 1, route.length));
			Collections.reverse(first);
			Collections.reverse(last);
			List<Short> both = new ArrayList<Short>();
			both.addAll(first);
			both.addAll(last);
			for (int n = 0; n < first.size(); n++) {
				route[n] = both.get(n);
			}

			for (int n = 0; n < last.size(); n++) {
				route[k + 1 + n] = both.get(first.size() + n);
			}
		}
	}
}
