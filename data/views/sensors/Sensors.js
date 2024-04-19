import AbstractView from "../AbstractView.js";
import { getSensorTypes } from "./SensorHelpers.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle("Sensors");
    }

    getSensors = async () => await fetch("/api/sensors")
        .then(response => response.json())
        .then(json => json ? json : {});

    async getHtml() {
        return `
            <h1>Sensors</h1>
            <table>
                <thead><tr><td>Name</td><td>connector</td><td>connection</td><td>Type</td><td></td></thead>
                <tbody>
                ${
                    Object.values(await this.getSensors())
                    .map(sensor => `
                    <tr>
                        <td>${sensor.options.name}</td>
                        <td>${sensor.options.connector}</td>
                        <td>${sensor.options.connection}</td>
                        <td>${sensor.type}</td>
                        <td><ul>
                            <li><a href="/sensors/${sensor.uuid}" data-link="">Details</a></li>
                            <li><a href="/sensors/${sensor.uuid}/edit" data-link="">Edit</a></li>
                            <li><a href="/sensors/${sensor.uuid}/delete" data-link="">Delete</a></li>
                        </ul></td>
                    </tr>`)
                    .join("\n")
                }
                </tbody>
            </table>

            <h2>Add new sensors</h2>
            <ul>
            ${
                Object.entries(await getSensorTypes())
                .map(([k, v]) => `<li>Add <a href="/sensors/add/${k}" data-link="">${v}</a></li>`)
                .join("\n")
            }
            </ul>
        `;
    }
}
