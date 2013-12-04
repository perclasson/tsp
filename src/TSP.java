import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Random;

public class TSP {
	private double[][] coordinates;
	private int[][] distances;
	private static boolean benchmark;

	public TSP() {
		long deadline = System.currentTimeMillis();

		try {
			coordinates = readCoordinates();
		} catch (IOException e) {
			e.printStackTrace();
		}

		distances = calculateEuclideanDistance();
		Short[] route = twoOpt(nearestNeighbourConstruct(), deadline + 1500);

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.length; i++) {
				System.out.println(route[i]);
			}
		}
	}

	private Short[] twoOpt(Short[] route, long deadline) {
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
		return existingRoute;
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

	private int[][] calculateEuclideanDistance() {
		int[][] euclideanDistances = new int[coordinates.length][];
		for (int i = 0; i < coordinates.length; i++) {
			euclideanDistances[i] = new int[coordinates.length];
			for (int j = 0; j < i; j++) {
				int dist = euclidianDistance(coordinates[i], coordinates[j]);
				euclideanDistances[i][j] = dist;
				euclideanDistances[j][i] = dist;
			}
		}
		return euclideanDistances;
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
