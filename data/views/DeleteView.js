// frontend/static/js/views/Dashboard.js
import AbstractView from "./AbstractView.js";

const DeleteView = (type) => class extends AbstractView {
    constructor(params) {
        super(params);
    }

    async getHtml() {
        return `
            <h1>Delete ${type} with id ${this.params.uuid}</h1>
            <p>Are you sure you wish to delete ${type} with id ${this.params.uuid}?</p>
            <form action="/api/${type}s/delete" method="post">
                <input type="hidden" name="uuid" value="${this.params.uuid}"/>
                <input type="submit" value="Yes"/>
                <a href="/sensors">No</a>
            </form>
        `;
    }
}

export default DeleteView;
