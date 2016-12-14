function a = plotposition(beacons, estimate, actual)

xs = [estimate(1); actual(1); beacons(:,1)];
ys = [estimate(2); actual(2); beacons(:,2)];
color = zeros(size(beacons,1)+2, 3);
color(:,1)=1;
color(1,2)=1;
color(2,3)=1;

scatter(xs, ys, 50, color, 'filled')

a = 0;
end