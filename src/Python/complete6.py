import numpy as np
import pandas as pd
import plotly.graph_objs as go
import dash
from dash import dcc, html
import plotly.subplots as sp
import matplotlib.pyplot as plt
from tqdm import tqdm
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import cross_val_score

# Load datasets
features = pd.read_csv("C:/Users/masou/OneDrive/Desktop/Utrecht University/Multimedia Retrieval/Assignment/MMR/src/Python/features_final.txt", sep='\t')
labels = features['ClassName']
distance_matrix = pd.read_csv("C:/Users/masou/OneDrive/Desktop/Utrecht University/Multimedia Retrieval/Assignment/MMR/src/Python/0_0_0.700000.txt", sep=',', header=None)
distance_matrix = distance_matrix.drop(columns=distance_matrix.columns[-1])

# Sort indices based on distance
sorted_indices = np.argsort(distance_matrix.values, axis=1)
sorted_indices = np.delete(sorted_indices, 0, axis=1)

# Define selected classes to focus on
selected_classes = ["Bicycle", "Door", "Wheel", "Bus", "BuildingNonResidential"]

# Precompute all metrics to improve the speed for presentation
K_values = [1, 5, 10, 15, 20]
beta_values = [0.5, 1.0, 2.0]
K_tier_values = [1, 5, 10, 15, 20, 25]
Q_values = [0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500, 2000, 3000]
unique_classes = labels.unique()

# Precompute AUROC
scalar_feature_names = ['Diameter', 'BB_Diameter', 'BB_Volume', 'SurfaceArea',
                        'Volume', 'VolumeComps', 'Convexity', 'Eccentricity02',
                        'Eccentricity01', 'Eccentricity12', 'Compactness', 'Sphericity',
                        'CubeRootVolume', 'SquareRootArea', 'DiameterToCubeRootVolume',
                        'DiameterToSquareRootArea', 'DiameterSquared', 'BBVolumeCubeRoot',
                        'Rectangularity']

scalar_features = features[scalar_feature_names]

# Extract histogram features
histogram_features = {
    'Histogram_A3': features.iloc[:, 21:121],
    'Histogram_D1': features.iloc[:, 121:221],
    'Histogram_D2': features.iloc[:, 221:321],
    'Histogram_D3': features.iloc[:, 321:421],
    'Histogram_D4': features.iloc[:, 421:521]
}

# Standardize the features
scaler = StandardScaler()

# Scaling scalar features
scaled_scalar_features = pd.DataFrame(scaler.fit_transform(scalar_features), columns=scalar_features.columns)

# Scaling histogram features
scaled_histogram_features = {}
for name, histogram_data in histogram_features.items():
    scaled_histogram_features[name] = pd.DataFrame(scaler.fit_transform(histogram_data), columns=histogram_data.columns)

# Combine all features for visualization purposes
all_features = {'Scalar_' + feature: scaled_scalar_features[feature] for feature in scaled_scalar_features.columns}
for name, data in scaled_histogram_features.items():
    all_features[name] = data.mean(axis=1)  # Averaging histogram features to treat them like scalar for AUROC

# Convert combined features to DataFrame
combined_features = pd.DataFrame(all_features)

# One-vs-all label conversion for AUROC calculation
binary_labels = (labels == unique_classes[0]).astype(int)

# Calculate AUROC scores using cross-validation for each feature
auroc_results = {}
for feature_name in tqdm(combined_features.columns, desc="Calculating AUROC scores"):
    feature_data = combined_features[[feature_name]]
    lr_model = LogisticRegression(solver='liblinear')
    auc_scores = cross_val_score(lr_model, feature_data, binary_labels, cv=5, scoring='roc_auc')
    auroc_results[feature_name] = np.mean(auc_scores)

auroc_df = pd.DataFrame(list(auroc_results.items()), columns=['Feature Name', 'AUROC Score'])

# Filter for selected classes only
def filter_selected_classes(results):
    return {cls: value for cls, value in results.items() if cls in selected_classes}

# Precompute Precision
precision_results = {}
for K in K_values:
    precision_per_class = {class_name: [] for class_name in unique_classes}
    for query_idx in range(len(features)):
        query_class = labels.iloc[query_idx]
        retrieved_indices = sorted_indices[query_idx][:K]
        retrieved_classes = labels.iloc[retrieved_indices]
        relevant_retrieved = sum(retrieved_classes == query_class)
        precision = relevant_retrieved / K
        precision_per_class[query_class].append(precision)
    average_precision_per_class = {cls: np.mean(precisions) for cls, precisions in precision_per_class.items()}
    precision_results[K] = filter_selected_classes(average_precision_per_class)

# Precompute Recall
recall_results = {}
for K in K_values:
    recall_per_class = {class_name: [] for class_name in unique_classes}
    for query_idx in range(len(features)):
        query_class = labels.iloc[query_idx]
        retrieved_indices = sorted_indices[query_idx][:K]
        retrieved_classes = labels.iloc[retrieved_indices]
        true_positives = sum(retrieved_classes == query_class)
        relevant_items = sum(labels == query_class)
        recall = true_positives / relevant_items if relevant_items != 0 else 0.0
        recall_per_class[query_class].append(recall)
    average_recall_per_class = {cls: np.mean(recalls) for cls, recalls in recall_per_class.items()}
    recall_results[K] = filter_selected_classes(average_recall_per_class)

# Precompute F_beta Score
fbeta_results = {}
for beta in beta_values:
    fbeta_results[beta] = {}
    for K in K_values:
        fbeta_per_class = {class_name: [] for class_name in unique_classes}

        for query_idx in range(len(features)):
            query_class = labels.iloc[query_idx]
            retrieved_indices = sorted_indices[query_idx][:K]
            retrieved_classes = labels.iloc[retrieved_indices]

            true_positives = sum(retrieved_classes == query_class)
            relevant_items = sum(labels == query_class)
            precision = true_positives / K if K != 0 else 0
            recall = true_positives / relevant_items if relevant_items != 0 else 0

            if precision + recall > 0:
                fbeta = (1 + beta**2) * (precision * recall) / (beta**2 * precision + recall)
            else:
                fbeta = 0

            fbeta_per_class[query_class].append(fbeta)

        average_fbeta_per_class = {cls: np.mean(fbetas) for cls, fbetas in fbeta_per_class.items()}
        fbeta_results[beta][K] = filter_selected_classes(average_fbeta_per_class)

# Precompute K-tier
k_tier_results = {}
for k_tier in K_tier_values:
    k_tier_per_class = {class_name: [] for class_name in labels.unique()}
    for query_idx in range(len(features)):
        query_class = labels.iloc[query_idx]
        class_size = len(features[features['ClassName'] == query_class]) - 1
        if class_size <= 0:
            continue  # Skip if class size is zero or negative
        tier_size = int(class_size * (k_tier / 100))  # Assuming k_tier is a percentage
        if tier_size <= 0:
            tier_size = 1  # Ensure at least one item is retrieved
        retrieved_indices = sorted_indices[query_idx][:tier_size]
        retrieved_classes = labels.iloc[retrieved_indices]
        relevant_retrieved = sum(retrieved_classes == query_class)
        k_tier_metric = relevant_retrieved / class_size
        k_tier_per_class[query_class].append(k_tier_metric)
    average_k_tier_per_class = {cls: np.mean(metrics) for cls, metrics in k_tier_per_class.items()}
    k_tier_results[k_tier] = filter_selected_classes(average_k_tier_per_class)

# Precompute Mean Average Precision (MAP)
map_results = {}
for K in K_values:
    average_precisions = []

    for query_idx in tqdm(range(len(features)), desc=f"Calculating MAP for K={K}"):
        query_class = labels.iloc[query_idx]
        retrieved_indices = sorted_indices[query_idx][:K]
        retrieved_classes = labels.iloc[retrieved_indices]

        # Calculate precision at each relevant position
        num_relevant = 0
        precisions = []

        for i in range(K):
            if retrieved_classes.iloc[i] == query_class:
                num_relevant += 1
                precision_at_i = num_relevant / (i + 1)
                precisions.append(precision_at_i)

        # Calculate average precision for this query
        if num_relevant > 0:
            average_precision = np.mean(precisions)
        else:
            average_precision = 0
        average_precisions.append(average_precision)

    # Mean Average Precision across all queries
    map_value = np.mean(average_precisions)
    map_results[K] = map_value

# Precompute ROC
class_frequencies_map = labels.value_counts().to_dict()
sensitivities = []
specificities = []

for Qval in Q_values:
    total_sensitivity = 0
    total_specificity = 0

    for i in range(len(features)):
        query_class = labels.iloc[i]
        c = class_frequencies_map[query_class] - 1  # Exclude the query itself

        retrieved_indices = sorted_indices[i][:Qval]
        retrieved_classes = labels.iloc[retrieved_indices]

        true_positive = sum(retrieved_classes == query_class)
        false_positive = sum(retrieved_classes != query_class)
        true_negative = sum(labels.iloc[sorted_indices[i][Qval:]] != query_class)
        false_negative = c - true_positive

        if (true_positive + false_negative) > 0:
            total_sensitivity += true_positive / (true_positive + false_negative)
        if (true_negative + false_positive) > 0:
            total_specificity += true_negative / (true_negative + false_positive)

    avg_sensitivity = total_sensitivity / len(features)
    avg_specificity = total_specificity / len(features)

    sensitivities.append(avg_sensitivity)
    specificities.append(1 - avg_specificity)

# Dash App Setup
app = dash.Dash(__name__)
server = app.server

app.layout = html.Div([
    html.H1("Interactive Metrics Dashboard", style={'textAlign': 'center'}),
    dcc.Dropdown(
        id='metric-selector',
        options=[
            {'label': 'AUROC', 'value': 'AUROC'},
            {'label': 'Precision', 'value': 'Precision'},
            {'label': 'Recall', 'value': 'Recall'},
            {'label': 'F_beta Score', 'value': 'F_beta'},
            {'label': 'K-tier', 'value': 'K_tier'},
            {'label': 'Mean Average Precision (MAP)', 'value': 'MAP'},
            {'label': 'ROC Curve', 'value': 'ROC'}
        ],
        value='AUROC',
        style={'width': '50%', 'margin': '0 auto'}
    ),
    dcc.Graph(id='metric-graph'),
])

# Callback to update graph based on selected metric
@app.callback(
    dash.dependencies.Output('metric-graph', 'figure'),
    [dash.dependencies.Input('metric-selector', 'value')]
)
def update_graph(selected_metric):
    if selected_metric == 'AUROC':
        fig = go.Figure(go.Bar(
            x=auroc_df['Feature Name'],
            y=auroc_df['AUROC Score'],
            marker=dict(color='skyblue'),
            name='AUROC Score'
        ))
        fig.add_trace(go.Scatter(
            x=auroc_df['Feature Name'],
            y=[0.5] * len(auroc_df),
            mode='lines',
            line=dict(color='red', dash='dash'),
            name='Random Guess (AUROC = 0.5)'
        ))
        fig.update_layout(
            title='AUROC Scores for Scalar and Histogram Features',
            xaxis_title='Feature Name',
            yaxis_title='AUROC Score',
            yaxis=dict(range=[0, 1]),
            xaxis=dict(tickangle=-45, tickfont=dict(size=10)),
            height=600,
            width=1000,
            showlegend=True
        )
    elif selected_metric == 'Precision':
        fig = sp.make_subplots(rows=1, cols=1)
        for K in K_values:
            avg_precision_per_class = precision_results[K]
            df_precision = pd.DataFrame(list(avg_precision_per_class.items()), columns=['Class', 'Average Precision'])
            trace = go.Bar(
                x=df_precision['Class'],
                y=df_precision['Average Precision'],
                name=f'K={K}',
                visible=False
            )
            fig.add_trace(trace)
        fig.data[0].visible = True
        steps = []
        for i, K in enumerate(K_values):
            visibility = [False] * len(fig.data)
            visibility[i] = True
            step = dict(
                method="update",
                args=[{"visible": visibility},
                      {"title": f"Average Precision per Class (K={K})"}],
                label=f'K={K}'
            )
            steps.append(step)
        sliders = [dict(
            active=0,
            currentvalue={"prefix": "K: "},
            pad={"t": 70, "b": 20},
            steps=steps
        )]
        fig.update_layout(
            sliders=sliders,
            title="Average Precision per Class",
            xaxis_title="Class",
            yaxis_title="Average Precision",
            xaxis_tickangle=-45,
            height=600,
            width=900,
            margin=dict(l=40, r=40, t=60, b=140),
            showlegend=True
        )
    elif selected_metric == 'Recall':
        fig = sp.make_subplots(rows=1, cols=1)
        for K in K_values:
            avg_recall_per_class = recall_results[K]
            df_recall = pd.DataFrame(list(avg_recall_per_class.items()), columns=['Class', 'Average Recall'])
            trace = go.Bar(
                x=df_recall['Class'],
                y=df_recall['Average Recall'],
                name=f'K={K}',
                visible=False
            )
            fig.add_trace(trace)
        fig.data[0].visible = True
        steps = []
        for i, K in enumerate(K_values):
            visibility = [False] * len(fig.data)
            visibility[i] = True
            step = dict(
                method="update",
                args=[{"visible": visibility},
                      {"title": f"Average Recall per Class (K={K})"}],
                label=f'K={K}'
            )
            steps.append(step)
        sliders = [dict(
            active=0,
            currentvalue={"prefix": "K: "},
            pad={"t": 70, "b": 20},
            steps=steps
        )]
        fig.update_layout(
            sliders=sliders,
            title="Average Recall per Class",
            xaxis_title="Class",
            yaxis_title="Average Recall",
            xaxis_tickangle=-45,
            height=600,
            width=900,
            margin=dict(l=40, r=40, t=60, b=140),
            showlegend=True
        )
    elif selected_metric == 'F_beta':
        fig = sp.make_subplots(rows=1, cols=1)
        traces = []
        for beta in beta_values:
            for K in K_values:
                avg_fbeta_per_class = fbeta_results[beta][K]
                df_fbeta = pd.DataFrame(list(avg_fbeta_per_class.items()), columns=['Class', 'F_beta Score'])
                trace = go.Bar(
                    x=df_fbeta['Class'],
                    y=df_fbeta['F_beta Score'],
                    name=f'Beta={beta}, K={K}',
                    visible=False
                )
                traces.append(trace)
                fig.add_trace(trace)
        fig.data[0].visible = True
        steps = []
        idx = 0
        for beta in beta_values:
            for K in K_values:
                visibility = [False] * len(fig.data)
                visibility[idx] = True
                step = dict(
                    method="update",
                    args=[{"visible": visibility},
                          {"title": f"F_beta Score per Class (Beta={beta}, K={K})"}],
                    label=f'Beta={beta}, K={K}'
                )
                steps.append(step)
                idx += 1
        sliders = [dict(
            active=0,
            currentvalue={"prefix": "Beta and K: "},
            pad={"t": 50},
            steps=steps
        )]
        fig.update_layout(
            sliders=sliders,
            title="F_beta Score per Class",
            xaxis_title="Class",
            yaxis_title="F_beta Score",
            xaxis_tickangle=-45,
            height=600,
            width=900,
            showlegend=False
        )
    elif selected_metric == 'K_tier':
        fig = sp.make_subplots(rows=1, cols=1)
        for k_tier in K_tier_values:
            avg_k_tier_per_class = k_tier_results[k_tier]
            df_k_tier = pd.DataFrame(list(avg_k_tier_per_class.items()), columns=['Class', 'K-Tier Metric'])
            trace = go.Bar(
                x=df_k_tier['Class'],
                y=df_k_tier['K-Tier Metric'],
                name=f'K-Tier={k_tier}',
                visible=False
            )
            fig.add_trace(trace)
        fig.data[0].visible = True
        steps = []
        for i, k_tier in enumerate(K_tier_values):
            visibility = [False] * len(fig.data)
            visibility[i] = True
            step = dict(
                method="update",
                args=[{"visible": visibility},
                      {"title": f"K-Tier Metric per Class (K-Tier={k_tier})"}],
                label=f'K-Tier={k_tier}'
            )
            steps.append(step)
        sliders = [dict(
            active=0,
            currentvalue={"prefix": "K-Tier: "},
            pad={"t": 50},
            steps=steps
        )]
        fig.update_layout(
            sliders=sliders,
            title="K-Tier Metric per Class",
            xaxis_title="Class",
            yaxis_title="K-Tier Metric",
            xaxis_tickangle=-45,
            height=600,
            width=1000,
            showlegend=True
        )
    elif selected_metric == 'MAP':
        fig = go.Figure(go.Scatter(
            x=K_values,
            y=[map_results[K] for K in K_values],
            mode='lines+markers',
            name='MAP'
        ))
        fig.update_layout(
            title='Mean Average Precision (MAP) vs K',
            xaxis_title='K',
            yaxis_title='MAP',
            height=600,
            width=900,
            showlegend=True
        )
    elif selected_metric == 'ROC':
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=specificities, y=sensitivities, mode='lines+markers', name='ROC Curve', line=dict(color='blue')))
        fig.add_trace(go.Scatter(x=[0, 1], y=[0, 1], mode='lines', line=dict(color='red', dash='dash'), name='Random Guess'))

        fig.update_layout(
            title='Receiver Operating Characteristic (ROC) Curve',
            xaxis_title='False Positive Rate (1 - Specificity)',
            yaxis_title='True Positive Rate (Sensitivity)',
            xaxis=dict(range=[0, 1]),
            yaxis=dict(range=[0, 1]),
            height=600,
            width=900,
            showlegend=True
        )
    else:
        fig = go.Figure()

    return fig

if __name__ == '__main__':
    app.run_server(debug=True)
