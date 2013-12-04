import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class TSP {
	private double[][] coordinates;
	private Integer[][] distances;
	private static boolean benchmark;
	private static int noNeighbours = 15;
	private List<List<Short>> neighbours;

	public TSP() {
		long deadline = System.currentTimeMillis();

		try {
			coordinates = readCoordinates();
		} catch (IOException e) {
			e.printStackTrace();
		}

		calculateAllDistances();
		createNearestNeighbours();
		Short[] route = twoOpt(nearestNeighbourConstruct(), deadline + 1500);

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.length; i++) {
				System.out.println(route[i]);
			}
		}
	}

	private void createNearestNeighbours() {
		if (distances.length <= noNeighbours)
			noNeighbours = distances.length - 1;

		List<Short> rangeList = new ArrayList<Short>();
		for (short i = 0; i < distances.length; i++) {
			rangeList.add(i);
		}
		neighbours = new ArrayList<List<Short>>(distances.length);
		for (int i = 0; i < distances.length; i++) {
			List<Short> neighbourList = new ArrayList<Short>(rangeList);
			final Integer[] comp = distances[i];
			Collections.sort(neighbourList, new Comparator<Short>() {
				public int compare(Short a, Short b) {
					return comp[a].compareTo(comp[b]);
				}
			});
			neighbours.add(new ArrayList<Short>(neighbourList.subList(0, noNeighbours)));
		}
	}

	private Short[] twoOpt(Short[] route, long deadline) {
		boolean improved = true;
		search: while (improved) {
			improved = false;
			for (int i = 0; i < route.length; i++) {
				for (Short k : neighbours.get(i)) {
					if (System.currentTimeMillis() >= deadline)
						break search;
					int a = Math.min(i, k);
					int b = Math.max(i, k);
					if (a != b && isGain(route, a, b)) {
						twoOptSwap(route, a, b);
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
		Collections.reverse(routeList.subList(i, k + 1));
	}

	private Short[] nearestNeighbourConstruct() {
		Short[] newRoute = new Short[distances.length];
		boolean[] visited = new boolean[distances.length];

		int firstVertex = 0;
		newRoute[0] = (short) firstVertex;
		visited[firstVertex] = true;

		for (int i = 0; i < newRoute.length - 1; i++) {
			newRoute[i + 1] = findNearestNeighbour(newRoute[i], visited);
		}

		return newRoute;
	}

	private short findNearestNeighbour(int from, boolean[] visited) {
		for (Short to : neighbours.get(from)) {
			if (!visited[to]) {
				visited[to] = true;
				return to;
			}
		}

		int minCost = Integer.MAX_VALUE;
		short nearest = 0;
		for (short to = 0; to < distances.length; to++) {
			int cost = distances[from][to];
			if (!visited[to] && cost < minCost) {
				minCost = cost;
				nearest = to;
			}
		}
		visited[nearest] = true;
		return nearest;
	}

	private int calculateTotalDistance(Short[] route) {
		int sum = distances[route[route.length - 1]][route[0]];
		for (int i = 0; i < route.length - 1; i++) {
			sum += distances[route[i]][route[(i + 1)]];
		}
		return sum;
	}

	private void calculateAllDistances() {
		distances = new Integer[coordinates.length][];
		for (int i = 0; i < coordinates.length; i++) {
			distances[i] = new Integer[coordinates.length];
			distances[i][i] = Integer.MAX_VALUE;
			for (int j = 0; j < i; j++) {
				int dist = euclidianDistance(coordinates[i], coordinates[j]);
				distances[i][j] = dist;
				distances[j][i] = dist;
			}
		}
	}

	private int euclidianDistance(double[] i, double[] j) {
		return (int) Math.round(Math.sqrt(Math.pow(i[0] - j[0], 2) + Math.pow(i[1] - j[1], 2)));
	}

	private double[][] readCoordinates() throws IOException {
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		double[][] data;
		int n = Integer.parseInt(br.readLine());
		data = new double[n][2];
		for (int i = 0; i < n; i++) {
			String[] input = br.readLine().split(" ");
			data[i][0] = Double.parseDouble(input[0]);
			data[i][1] = Double.parseDouble(input[1]);
		}
		return data;
	}

	public static void main(String[] args) {
		if (args.length > 0 && args[0].equals("benchmark")) {
			benchmark = true;
		}
		new TSP();
	}
}
