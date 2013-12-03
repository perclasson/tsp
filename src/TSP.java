import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class TSP {
	private static final int TIME_LIMIT = 1500;
	private double[][] coordinates;
	private int[][] distances;
	private List<Short> route;
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
		route = twoOptSearch(nearestNeighbourRoute());

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.size(); i++) {
				System.out.println(route.get(i));
			}
		}
	}

	private List<Short> twoOptSearch(List<Short> existingRoute) {
		int bestDistance = calculateTotalDistance(existingRoute);
		search: while (true) {
			for (int i = 0; i < existingRoute.size() - 1; i++) {
				for (int k = i + 1; k < existingRoute.size(); k++) {
					if (System.currentTimeMillis() >= deadline)
						break search;
					List<Short> newRoute = new ArrayList<Short>();// (existingRoute);
					twoOptSwap(newRoute, i, k);
					int newDistance = calculateTotalDistance(newRoute);
					if (newDistance < bestDistance) {
						existingRoute = newRoute;
						bestDistance = newDistance;
						continue search;
					}
				}
			}
			break;
		}
		return existingRoute;
	}

	private void twoOptSwap(List<Short> route, int i, int k) {
		Collections.reverse(route.subList(i, k));
	}

	private List<Short> nearestNeighbourRoute() {
		List<Short> newRoute = new ArrayList<Short>();
		boolean[] visited = new boolean[distances.length];

		short firstVertex = 0;
		newRoute.add(firstVertex);
		visited[firstVertex] = true;

		for (int i = 0; i < newRoute.size() - 1; i++) {
			newRoute.add(findNearestNeighbour(newRoute.get(i), visited));
		}

		return newRoute;
	}

	private short findNearestNeighbour(int from, boolean[] visited) {
		int minCost = Integer.MAX_VALUE;
		short nearest = 0;
		for (short to = 0; to < distances.length; to++) {
			int cost = getDistance(from, to);
			if (!visited[to] && cost < minCost) {
				minCost = cost;
				nearest = to;
			}
		}
		visited[nearest] = true;
		return nearest;
	}

	private int calculateTotalDistance(List<Short> route) {
		int sum = getDistance(route.get(route.size() - 1), route.get(0));
		for (int i = 0; i < route.size() - 1; i++) {
			sum += getDistance(route.get(i), route.get(i + 1));
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
