import Dashboard from './views/Dashboard.js';
import Sensors from './views/sensors/Sensors.js';
import Sensor from './views/sensors/Sensor.js';
import Calculators from './views/calculators/Calcs.js';
import Calculator from './views/calculators/Calc.js';
import DeleteView from './views/DeleteView.js';
import SensorEdit from './views/sensors/SensorEdit.js';
import SensorAdd from './views/sensors/SensorAdd.js';
import CalculatorAdd from './views/calculators/CalcAdd.js';
import CalculatorEdit from './views/calculators/CalcEdit.js';
import MQTT from './views/mqtt/MQTT.js';
import FileSystem from './views/diagnostics/FS.js';
import Upload from './views/diagnostics/FSUpload.js';

const pathToRegex = path => new RegExp("^" + path.replace(/\//g, "\\/").replace(/:(\w+)/g, '(?<$1>.+)') + "$");
const router = async () => {
    const routes = [
        { path: "/", view: Dashboard },
        { path: "/mqtt", view: MQTT },
        { path: "/calculators", view: Calculators },
        { path: "/calculators/add/:type", view: CalculatorAdd },
        { path: "/calculators/:uuid/edit", view: CalculatorEdit },
        { path: "/calculators/:uuid/delete", view: DeleteView("calculator") },
        { path: "/calculators/:uuid", view: Calculator },
        { path: "/sensors", view: Sensors },
        { path: "/sensors/add/:type", view: SensorAdd },
        { path: "/sensors/:uuid/edit", view: SensorEdit },
        { path: "/sensors/:uuid/delete", view: DeleteView("sensor") },
        { path: "/sensors/:uuid", view: Sensor },
        { path: "/filesystem", view: FileSystem },
        { path: "/filesystem/delete", view: DeleteView("file") },
        { path: "/filesystem/upload", view: Upload },
    ];

    const potentialMatches = routes.map(route => {
        return {
            route,
            result: location.pathname.match(pathToRegex(route.path))
        };
    });

    let match = potentialMatches.find(potentialMatch => potentialMatch.result !== null);
    /* Route not found - return first route OR a specific "not-found" route */
    if (!match) {
        match = {
            route: routes[0],
            result: [location.pathname]
        };
    }
    console.log(match);

    const view = new match.route.view(match.result.groups);
    document.querySelector("#app").innerHTML = '<div class="lds-dual-ring"></div>';
    document.querySelector("#app").innerHTML = await view.getHtml();
    if (view.bindHandlers) {
        view.bindHandlers();
    }
    handleRangeInputs();
}
const navigateTo = url => {
    history.pushState(null, null, url);
    router();
};
const handleRangeInputs = () => document.querySelectorAll("input[type=range]").forEach(el => {
        if (!el.id) {
            return;
        }

        const output = document.querySelector(`#${el.id}_output`);
        output.textContent = el.value;
        el.addEventListener("input", (event) => output.textContent = event.target.value);
    })

document.addEventListener("DOMContentLoaded", () => {
    document.body.addEventListener("click", e => {
        if (e.target.matches("[data-link]")) {
            e.preventDefault();
            navigateTo(e.target.href);
        }
    });

    /* Document has loaded -  run the router! */
    router();
});

window.addEventListener("popstate", router);

