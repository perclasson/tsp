import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class TSP {
	private static final int TIME_LIMIT = 1500;
	private double[][] coordinates;
	private int[][] distances;
	private Short[] route;
	private long deadline;
	private static boolean benchmark;

	public TSP() {
		deadline = System.currentTimeMillis() + TIME_LIMIT;

		try {
			coordinates = readCoordinates();
		} catch (IOException e) {
			e.printStackTrace();
		}

		distances = calculateEuclideanDistance();
		route = twoOptSearch(nearestNeighbour());

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.length; i++) {
				System.out.println(route[i]);
			}
		}
	}

	private Short[] twoOptSearch(Short[] existingRoute) {
		search: while (true) {
			int bestDistance = calculateTotalDistance(existingRoute);
			for (int i = 0; i < existingRoute.length - 1; i++) {
				for (int k = i + 1; k < existingRoute.length; k++) {
					if (System.currentTimeMillis() >= deadline)
						break search;
					Short[] newRoute = existingRoute.clone();
					twoOptSwap(newRoute, i, k);
					int newDistance = calculateTotalDistance(newRoute);
					if (newDistance < bestDistance) {
						existingRoute = newRoute;
						continue search;
					}
				}
			}
			break;
		}
		return existingRoute;
	}

	private Short[] twoOptSwap(Short[] route, int i, int k) {
		List<Short> routeList = Arrays.asList(route);
		Collections.reverse(routeList.subList(i, k));
		return (Short[]) routeList.toArray();
	}

	private Short[] nearestNeighbour() {
		Short[] newRoute = new Short[distances.length];
		boolean[] visited = new boolean[distances.length];

		int firstVertex = 0;
		newRoute[0] = (short) firstVertex;
		visited[firstVertex] = true;

		for (int i = 0; i < newRoute.length - 1; i++) {
			newRoute[i + 1] = findNearestVertex(newRoute[i], visited);
		}

		return newRoute;
	}

	private short findNearestVertex(int from, boolean[] visited) {
		int minCost = Integer.MAX_VALUE;
		int nearest = 0;
		for (int to = 0; to < distances.length; to++) {
			int cost = getDistance(from, to);
			if (!visited[to] && cost < minCost) {
				minCost = cost;
				nearest = to;
			}
		}
		visited[nearest] = true;
		return (short) nearest;
	}

	private int calculateTotalDistance(Short[] route) {
		int sum = 0;
		for (int i = 0; i < route.length; i++) {
			sum += getDistance(route[i], route[(i + 1) % route.length]);
		}
		return sum;
	}

	private int[][] calculateEuclideanDistance() {
		int[][] euclideanDistances = new int[coordinates.length][];
		for (int i = 0; i < coordinates.length; i++) {
			euclideanDistances[i] = new int[i + 1];
			for (int j = 0; j < i; j++) {
				euclideanDistances[i][j] = euclidianDistance(coordinates[i], coordinates[j]);
			}
		}
		return euclideanDistances;
	}

	private int euclidianDistance(double[] i, double[] j) {
		return (int) Math.round(Math.sqrt(Math.pow(i[0] - j[0], 2) + Math.pow(i[1] - j[1], 2)));
	}

	private int getDistance(int i, int j) {
		if (j > i) {
			return distances[j][i];
		} else {
			return distances[i][j];
		}
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
